import KBEngine
from KBEDebug import *

class Space( KBEngine.Entity ):

    def __init__(self):
        KBEngine.Entity.__init__(self)
        KBEngine.globalData["space_%i" % self.spaceID] = self.base

    def onEnter(self, entityMailbox):
        pass
        #DEBUG_MSG('Space::onEnter space[%d] entityID = %i.' % (self.spaceID, entityMailbox.id))

    def onLeave(self, entityID):
        pass
        #DEBUG_MSG('Space::onLeave space[%d] entityID = %i.' % (self.spaceID, entityID))
