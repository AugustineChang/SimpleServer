import KBEngine
from KBEDebug import *

class GeneralObj( KBEngine.Entity ):
    def __init__(self):
        KBEngine.Entity.__init__(self)
        DEBUG_MSG("GeneralObj createCell %i" % self.id)
        self.isGrabbed = 0

    def pickupObj(self, callerEntityID, avatarID, isLeft, relativePos):
        if self.isGrabbed == 1:
            return
        self.isGrabbed = 1
        self.allClients.onPickupObj(avatarID, isLeft, relativePos)

        #dataName = "DebugHand" + str(isLeft)
        #if KBEngine.globalData.has_key(dataName):
        #    debugHand = KBEngine.globalData[dataName]
        #    self.allClients.onPickupObj(debugHand.avatarEntityID, isLeft, relativePos)

    def dropObj(self, callerEntityID, avatarID, isLeft):
        if self.isGrabbed == 0:
            return
        self.isGrabbed = 0
        self.allClients.onDropObj(avatarID, isLeft)

        #dataName = "DebugHand" + str(isLeft)
        #if KBEngine.globalData.has_key(dataName):
        #    debugHand = KBEngine.globalData[dataName]
        #    self.allClients.onDropObj(debugHand.avatarEntityID, isLeft)

    def proximityObj(self, callerEntityID, avatarID, isLeft):
        if self.isGrabbed == 0:
            return
        self.isGrabbed = 0
        self.allClients.onProximityObj(avatarID, isLeft)

        #dataName = "DebugHand" + str(isLeft)
        #if KBEngine.globalData.has_key(dataName):
        #    debugHand = KBEngine.globalData[dataName]
        #    self.allClients.onProximityObj(debugHand.avatarEntityID, isLeft)

    def leaveObj(self, callerEntityID, avatarID, isLeft):
        if self.isGrabbed == 0:
            return
        self.isGrabbed = 0
        self.allClients.onLeaveObj(avatarID, isLeft)

        #dataName = "DebugHand" + str(isLeft)
        #if KBEngine.globalData.has_key(dataName):
        #    debugHand = KBEngine.globalData[dataName]
        #    self.allClients.onLeaveObj(debugHand.avatarEntityID, isLeft)
