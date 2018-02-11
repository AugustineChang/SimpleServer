import KBEngine
import d_Maps
import d_MapObjs
import ConstDefine
from KBEDebug import *

class Space(KBEngine.Entity):

    def __init__(self):
        KBEngine.Entity.__init__(self)
        self.createCellEntityInNewSpace(None)

        self.mapKey = self.cellData["mapKey"]
        self.mapName = d_Maps.Maps.get(self.mapKey)["mapName"]
        self.avatars = []
        self.guideAvatars = []
        self.otherAvatars = []

        self.parseSpaceObjectDatas()

    def parseSpaceObjectDatas(self):
        self.tempObjsList = []
        tempData = d_MapObjs.MapObjs.get(self.mapKey)
        for readData in tempData:
            oneData = {}
            oneData['objTypeName'] = d_MapObjs.objTypes[readData[0]]
            oneData['position'] = readData[1]
            oneData['direction'] = readData[2]
            self.tempObjsList.append( oneData )

    def spawnSpaceObjects(self, tid):
        if len(self.tempObjsList) > 0:
            objData = self.tempObjsList.pop(0)
            objData['spaceToSpawn'] = self.cell
            KBEngine.createBaseAnywhere( "GeneralObj", objData )
        else:
            del self.tempObjsList
            self.delTimer(tid)

    def loginToSpace(self, avatarEntity):
        if self.cell is None:
            ERROR_MSG("Space::loginToSpace: space dont have cell part ,space=%s" % self.mapName)
            return False

        self.onEnter(avatarEntity)
        return True

    def logoutSpace(self, avatarID, playerIndex):
        self.onLeave(avatarID, playerIndex)

    def onGetCell(self):
        KBEngine.globalData["SpaceManager"].onSpaceGetCell(self.mapKey, self.spaceKey, self)
        self.addTimer(0.1, 0.1, ConstDefine.TIMER_TYPE_CREATE_OBJECTS)

    def debugSpawn(self):
        dic = { "name":"Debug", "isMale":0, "characterType":0, "clothesType":0, "hairType":0, "glassesType":0, "position": ( 8.0, 0.5, -1.0) }
        KBEngine.createBaseAnywhere("Avatar", dic, self.onCreateDebugAvatar)

    def onCreateDebugAvatar(self, avatar):
        self.addTimer(2, 1, ConstDefine.TIMER_TYPE_TEST_NEAROBJ)
        self.tempAvatar = avatar

    def onLoseCell(self):
        KBEngine.globalData["SpaceManager"].onSpaceLoseCell(self.mapKey, self.spaceKey)

    def onTimer(self, tid, userArg):
        if userArg == ConstDefine.TIMER_TYPE_CREATE_OBJECTS:
            self.spawnSpaceObjects(tid)
        if userArg == ConstDefine.TIMER_TYPE_TEST_NEAROBJ:
            self.tempAvatar.loginToSpace(1)
            del self.tempAvatar
            self.delTimer(tid)

    def onEnter(self, entityMailbox):
        firstNoneIndex = self.getFirstNone( self.avatars )
        self.insertToList(self.avatars, entityMailbox, firstNoneIndex)

        auth = entityMailbox.cellData["authority"]
        nextAuthIndex = 0
        if auth == 1:#guide
            nextAuthIndex = self.getFirstNone( self.guideAvatars )
            self.insertToList(self.guideAvatars, entityMailbox, nextAuthIndex)
        elif auth == 0:
            nextAuthIndex = self.getFirstNone( self.otherAvatars )
            self.insertToList(self.otherAvatars, entityMailbox, nextAuthIndex)

        DEBUG_MSG("Space::onEnter: avatar.id=%i playerindex=%i authIndex=%i" % (entityMailbox.id, firstNoneIndex, nextAuthIndex))
        trans = self.getSpawnTransform(nextAuthIndex, auth)
        entityMailbox.joinedInSpace(self.cell , self.mapKey, self.spaceKey, firstNoneIndex, trans[0], trans[1])

        if self.cell is not None:
            self.cell.onEnter(entityMailbox)

    def onLeave(self, avatarID, playerIndex):
        self.deleteFromList( self.avatars, avatarID, True )
        self.deleteFromList( self.guideAvatars, avatarID, False )
        self.deleteFromList( self.otherAvatars, avatarID, False )

        DEBUG_MSG("Space::onLeave: avatar.id=%i playerindex=%i" % (avatarID, playerIndex))
        if self.cell is not None:
            self.cell.onLeave(avatarID)

    def getSpawnTransform(self, authIndex, authority):
        spawnData = d_Maps.SpawnPoints[self.mapKey]
        pointsList = None
        if authority == 0:
            pointsList = spawnData["visitor"]
        else:
            pointsList = spawnData["guide"]
        listLen = len(pointsList)
        realIndex = authIndex % listLen
        return pointsList[realIndex]

    def getFirstNone(self, avatarlist):
        counter = 0
        for ele in avatarlist:
            if ele is None:
                return counter
            else:
                counter += 1
        return counter

    def insertToList(self, avatarlist, avatar, firstNoneIndex):
        length = len(avatarlist)
        if firstNoneIndex >= length:
            avatarlist.append(avatar)
        else:
            avatarlist[firstNoneIndex] = avatar

    def deleteFromList(self, avatarlist, avatarID, isDestroy):
        counter = 0
        length = len(avatarlist)
        for ele in avatarlist:
            if ele is not None and ele.id == avatarID:
                break
            counter += 1

        if counter < length and avatarlist[counter] is not None:
            if isDestroy == True:
                avatarlist[counter].leavedFromSpace()
            avatarlist[counter] = None
