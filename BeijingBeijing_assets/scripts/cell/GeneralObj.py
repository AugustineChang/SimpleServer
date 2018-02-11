import KBEngine
from KBEDebug import *

class GeneralObj( KBEngine.Entity ):
    def __init__(self):
        KBEngine.Entity.__init__(self)
        DEBUG_MSG("GeneralObj createCell %i" % self.id)
        self.isGrabbed = 0

    def pickupObj(self, callerEntityID, avatarID, isLeft):
        if self.isGrabbed == 1:
            return
        self.isGrabbed = 1
        self.allClients.onPickupObj(avatarID, isLeft)

    def dropObj(self, callerEntityID, avatarID, isLeft):
        if self.isGrabbed == 0:
            return
        self.isGrabbed = 0
        self.allClients.onDropObj(avatarID, isLeft)
