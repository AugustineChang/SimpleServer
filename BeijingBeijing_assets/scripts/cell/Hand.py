import KBEngine
from KBEDebug import *

class Hand( KBEngine.Entity ):

    def __init__(self):
        KBEngine.Entity.__init__(self)
        DEBUG_MSG("Hand createCell %i" % self.id)

        ####################################################
        if self.isDebug == 1:
            dataName = "DebugHand" + str(self.isLeft)
            KBEngine.globalData[dataName] = self
        ####################################################

    def handInputSync(self, callerEntityID, key, value):
        if key == 0:
            #DEBUG_MSG("Trigger val=%i left=%i"% (value, self.isLeft))
            self.triggerState = value
        elif key == 1:
            #DEBUG_MSG("grip val=%i left=%i"% (value, self.isLeft))
            self.gripState = value

        #####################################
        if self.isDebug == 0:
            dataName = "DebugHand" + str(self.isLeft)
            if KBEngine.globalData.has_key(dataName):
                debugHand = KBEngine.globalData[dataName]
                debugHand.handInputSync(callerEntityID,key,value)

    def handAxisSync(self, callerEntityID, key, value):
        if key == 2:
            self.thumbstickAxis = value
        #####################################
        if self.isDebug == 0:
            dataName = "DebugHand" + str(self.isLeft)
            if KBEngine.globalData.has_key(dataName):
                debugHand = KBEngine.globalData[dataName]
                debugHand.handAxisSync(callerEntityID,key,value)

    def handLaserStateSync(self, callerEntityID, laser):
        self.otherClients.onGetLaserState(laser)
        #####################################
        if self.isDebug == 0:
            dataName = "DebugHand" + str(self.isLeft)
            if KBEngine.globalData.has_key(dataName):
                debugHand = KBEngine.globalData[dataName]
                debugHand.handLaserStateSync(callerEntityID,laser)

    def handGrabStateSync(self, callerEntityID, grab):
        self.otherClients.onGetGrabState(grab)
        #####################################
        if self.isDebug == 0:
            dataName = "DebugHand" + str(self.isLeft)
            if KBEngine.globalData.has_key(dataName):
                debugHand = KBEngine.globalData[dataName]
                debugHand.handGrabStateSync(callerEntityID,grab)

    def updateHandLocation(self, callerEntityID, pos):
        self.position = pos
        #####################################
        if self.isDebug == 0:
            dataName = "DebugHand" + str(self.isLeft)
            if KBEngine.globalData.has_key(dataName):
                debugHand = KBEngine.globalData[dataName]
                debugHand.updateHandLocation(callerEntityID,pos)

    def updateHandDirection(self, callerEntityID, rot):
        self.direction = rot
        #####################################
        if self.isDebug == 0:
            dataName = "DebugHand" + str(self.isLeft)
            if KBEngine.globalData.has_key(dataName):
                debugHand = KBEngine.globalData[dataName]
                debugHand.updateHandDirection(callerEntityID,rot)
