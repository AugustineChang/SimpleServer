import KBEngine
from KBEDebug import *

class GeneralObj( KBEngine.Entity ):
    def __init__(self):
        KBEngine.Entity.__init__(self)
        DEBUG_MSG("GeneralObj createBase %i" % self.id)
        self.createCellEntity(self.spaceToSpawn)

    def onLoseCell(self):
        DEBUG_MSG("GeneralObj DestroyBase %i" % self.id)
        self.destroy(False, False)
