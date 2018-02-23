import KBEngine
import ConstDefine
from KBEDebug import *

class Avatar(KBEngine.Entity):
    def __init__(self):
        KBEngine.Entity.__init__(self)

        ####################################################
        #if self.isDebug == 1:
        #    KBEngine.globalData["DebugAvatar"] = self
        #elif self.isDebug == 0:
        #    self.addTimer(0.1,0.1,ConstDefine.TIMER_TYPE_MOVE_DEBUG_AVATAR)
        ####################################################

    def onTimer(self, tid, userArg):
        pass
        #if userArg == ConstDefine.TIMER_TYPE_MOVE_DEBUG_AVATAR:
        #    if self.isDebug == 0 and KBEngine.globalData.has_key("DebugAvatar"):
        #        #KBEngine.globalData["DebugAvatar"].position = self.position
        #        KBEngine.globalData["DebugAvatar"].direction = self.direction

    def onGetWitness(self):
        """
		KBEngine method.
		绑定了一个观察者(客户端)
		"""
        DEBUG_MSG("Avatar::onGetWitness: %i." % self.id)

    def onLoseWitness(self):
        """
		KBEngine method.
		解绑定了一个观察者(客户端)
		"""
        DEBUG_MSG("Avatar::onLoseWitness: %i." % self.id)

    def cameraPosSync(self, callerEntityID, pos):
        self.cameraPos = pos
        #####################################
        #if self.isDebug == 0 and KBEngine.globalData.has_key("DebugAvatar"):
        #    KBEngine.globalData["DebugAvatar"].cameraPosSync(callerEntityID,pos)

    def cameraRotSync(self, callerEntityID, rot):
        self.cameraRot = rot
        #####################################
        #if self.isDebug == 0 and KBEngine.globalData.has_key("DebugAvatar"):
        #    KBEngine.globalData["DebugAvatar"].cameraRotSync(callerEntityID,rot)

    def characterMoveSync(self, callerEntityID, speed):
        self.characterMoveSpeed = speed
        #####################################
        #if self.isDebug == 0 and KBEngine.globalData.has_key("DebugAvatar"):
        #    KBEngine.globalData["DebugAvatar"].characterMoveSync(callerEntityID,speed)

    def lipDataSync(self, callerEntityID, lipData):
        self.otherClients.onGetLipData(lipData)

    def voiceChatSync(self, callerEntityID, voiceData):
        self.otherClients.onGetVoiceData(voiceData)

    def uploadPingValue(self, callerEntityID, ping):
        self.pingValue = ping

    def doCustomAction(self, callerEntityID, customStr):
        self.customCmd = customStr
        #####################################
        #if self.isDebug == 0 and KBEngine.globalData.has_key("DebugAvatar"):
        #    KBEngine.globalData["DebugAvatar"].doCustomAction(callerEntityID,customStr)
