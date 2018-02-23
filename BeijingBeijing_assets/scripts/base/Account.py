# -*- coding: utf-8 -*-
import KBEngine
import ConstDefine
from KBEDebug import *

class Account(KBEngine.Proxy):
	def __init__(self):
		KBEngine.Proxy.__init__(self)
		self.activeAvatar = None

	def onTimer(self, id, userArg):
		"""
		KBEngine method.
		使用addTimer后， 当时间到达则该接口被调用
		@param id		: addTimer 的返回值ID
		@param userArg	: addTimer 最后一个参数所给入的数据
		"""
		pass
		#DEBUG_MSG(id, userArg)
		#if userArg == ConstDefine.TIMER_TYPE_AUTO_LOGIN:
		#	self.debugAvatar.loginToSpace(1)
		#	self.delTimer(id)

	def onEntitiesEnabled(self):
		"""
		KBEngine method.
		该entity被正式激活为可使用， 此时entity已经建立了client对应实体， 可以在此创建它的
		cell部分。
		"""
		INFO_MSG("account[%i] entities enable. mailbox:%s" % (self.id, self.client))

	def onLogOnAttempt(self, ip, port, password):
		"""
		KBEngine method.
		客户端登陆失败时会回调到这里
		"""
		#INFO_MSG(ip, port, password)
		if self.activeAvatar == None:
			return KBEngine.LOG_ON_ACCEPT
		else:
			return KBEngine.LOG_ON_REJECT

	def onClientDeath(self):
		"""
		KBEngine method.
		客户端对应实体已经销毁
		"""
		DEBUG_MSG("Account[%i].onClientDeath:" % self.id)

		if self.activeAvatar is not None:
			self.activeAvatar.accountEntity = None
		self.activeAvatar = None
		self.destroy()

	def onDestroy(self):
		"""
		KBEngine method.
		entity销毁
		"""
		DEBUG_MSG("Account::onDestroy: %i." % self.id)
		if self.activeAvatar is not None:
			self.activeAvatar.accountEntity = None
		self.activeAvatar = None

	def logoutGame(self):
		DEBUG_MSG("Account::logoutGame: logout~~")
		self.disconnect()

	def startGame(self, name , auth , charType):
		if self.activeAvatar is None:
			avatarDic = { "name":name, "authority":auth, "characterType":charType, "isDebug":0}
			KBEngine.createBaseAnywhere("Avatar", avatarDic, self.onGameStarted)
			##############################
			#debugAvatarDic = { "name":"DebugAvatar", "authority":0, "characterType":0 , "isDebug":1}
			#self.debugAvatar = KBEngine.createBaseLocally("Avatar" , debugAvatarDic)
			##############################
		else:
			self.client.onGameStarted(0)

	def onGameStarted(self, avatarEntity):
		if avatarEntity is None:
			self.client.onGameStarted(0)
		else:
			avatarEntity.accountEntity = self
			self.activeAvatar = avatarEntity
			self.client.onGameStarted(1)
			self.giveClientTo(self.activeAvatar)
			####################################
			#self.addTimer(2,1,ConstDefine.TIMER_TYPE_AUTO_LOGIN)
			####################################

	def onAvatarDestroy(self):
		#if KBEngine.globalData.has_key("DebugAvatar"):
		#	del KBEngine.globalData["DebugAvatar"]

		#if KBEngine.globalData.has_key("DebugHand0"):
		#	del KBEngine.globalData["DebugHand0"]

		#if KBEngine.globalData.has_key("DebugHand1"):
		#	del KBEngine.globalData["DebugHand1"]

		#self.debugAvatar.logoutSpace(True)
		self.destroy()
