import KBEngine
from KBEDebug import *

class Hand( KBEngine.Base ):

    def __init__(self):
        KBEngine.Base.__init__(self)
        self.hasCell = False
        self.isLeaveGame = False
        DEBUG_MSG("Hand createBase %i" % self.id)

    def createHandCell(self, space, pos, direct):
        if self.hasCell:
            DEBUG_MSG("Hand::createHandCell: already in space")
            return

        self.cellData["position"] = pos
        self.cellData["direction"] = direct
        self.createCellEntity(space)
        self.hasCell = True

    def destroyHand(self, isLeave):
        self.isLeaveGame = bool(isLeave)
        if self.hasCell:
            self.destroyCellEntity()

    def onLoseCell( self ):
        self.hasCell = False
        if self.isLeaveGame:
            self.destroy()
