import KBEngine
import ConstDefine
import socket
import struct
from KBEDebug import *

class Avatar(KBEngine.Proxy):
    def __init__(self):
        KBEngine.Proxy.__init__(self)
        self.spaceKey = None
        self.mapKey = None
        self.isLeaveGame = False
        self.addTimer(1,1,ConstDefine.TIMER_TYPE_CREATE_HANDS)
        self.isDebug = self.cellData["isDebug"]
        self.WaitForHands = None
        self.leftHand = None
        self.rightHand = None

    def onTimer(self, tid, userArg):
        if ConstDefine.TIMER_TYPE_CREATE_HANDS == userArg:
            self.leftHand = KBEngine.createBaseLocally("Hand",{"isLeft":1, "avatarEntityID":self.id, "isDebug":self.isDebug})
            self.rightHand = KBEngine.createBaseLocally("Hand",{"isLeft":0, "avatarEntityID":self.id, "isDebug":self.isDebug})
            self.delTimer(tid)

            if self.WaitForHands != None:
                self.loginToSpace(self.WaitForHands)
                del self.WaitForHands

    def loginToSpace(self, mapKey):
        if self.leftHand == None or self.rightHand == None:
            self.WaitForHands = mapKey
            return

        if self.spaceKey is not None:
            DEBUG_MSG("Avatar::loginToSpace: already in space")
            return
        KBEngine.globalData["SpaceManager"].loginToSpace(mapKey, self)

    def logoutSpace(self, isLeave):
        if self.spaceKey is None:
            DEBUG_MSG("Avatar::logoutSpace: No space to logout")
            return
        self.isLeaveGame = bool(isLeave)
        KBEngine.globalData["SpaceManager"].logoutSpace(self.mapKey, self.spaceKey, self.id, self.playerIndex)

    def logoutGame(self):
        DEBUG_MSG("Avatar::logoutGame: logout~~")
        if self.isDebug == 0:
            self.accountEntity.onAvatarDestroy()
        self.logoutSpace(True)

    def joinedInSpace(self, space, mapKey, spaceKey, playerIndex, pos, direct):
        if self.spaceKey == spaceKey:
            return

        self.cellData["ipAddr"] = socket.inet_ntoa(struct.pack('@L',self.clientAddr[0]))
        self.cellData["position"] = pos
        self.cellData["direction"] = direct
        self.cellData["playerIndex"] = playerIndex
        self.createCellEntity(space)
        self.leftHand.createHandCell(space, pos, direct)
        self.rightHand.createHandCell(space, pos, direct)

        self.spaceKey = spaceKey
        self.mapKey = mapKey
        self.playerIndex = playerIndex

    def leavedFromSpace(self):
        self.destroyCellEntity()
        self.spaceKey = None
        self.mapKey = None
        self.leftHand.destroyHand(1 if self.isLeaveGame else 0)
        self.rightHand.destroyHand(1 if self.isLeaveGame else 0)

    def onClientDeath(self):
        self.logoutGame()

    def onLoseCell(self):
        if self.isLeaveGame:
            if self.isDebug == 0:
                self.disconnect()
            self.destroy()

    def pingCall(self, index):
        self.client.echoPingCall( index )
