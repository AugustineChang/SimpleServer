import KBEngine
import copy
import ConstDefine
import d_Maps
from KBEDebug import *

class SpaceManager( KBEngine.Entity ):

    def __init__(self):
        KBEngine.Entity.__init__(self)
        self.spaces = {}
        self.tempMapKey = list(d_Maps.Maps.keys())
        self.addTimer(1,1,ConstDefine.TIMER_TYPE_CREATE_SPACES)
        KBEngine.globalData["SpaceManager"] = self

    def createSpaceOnTimer(self, tid):
        if len(self.tempMapKey) > 0:
            mapKey = self.tempMapKey.pop(0)
            self.createSpace(mapKey, 0, {})
        else:
            del self.tempMapKey
            self.delTimer(tid)

    def createSpace(self, mapKey, spaceKey, context):
        if spaceKey <=0:
            spaceKey = KBEngine.genUUID64()

        context = copy.copy(context)
        spaceData = d_Maps.Maps.get(mapKey)
        KBEngine.createBaseAnywhere( spaceData["entityType"], \
        {"mapKey":mapKey, "spaceKey":spaceKey, "context":context})

    def loginToSpace(self, mapKey, avatarEntity):
        return self.spaces[mapKey].loginToSpace(avatarEntity)

    def logoutSpace(self, mapKey, spaceKey, avatarID, playerIndex):
        ret = self.findSpace(mapKey, spaceKey)
        if ret is not None:
            ret.logoutSpace(avatarID, playerIndex)

    def findSpace(self, mapKey, spaceKey):
        space = self.spaces.get(mapKey)
        if space is not None:
            return space
        else:
            return None

    def onTimer(self, tid, userArg):
        if ConstDefine.TIMER_TYPE_CREATE_SPACES == userArg:
            self.createSpaceOnTimer(tid)

    def onSpaceGetCell(self, mapKey, spaceKey, spaceMailbox):
        self.spaces[mapKey] = spaceMailbox
        #DEBUG_MSG("spaces[%s]=%s" % (mapKey, self.spaces[mapKey]))

    def onSpaceLoseCell(self, mapKey, spaceKey):
        del self.spaces[mapKey]
