# -*- coding: utf-8 -*-
"""
Controls the Chopper

Last modification: 05.02.2021
Author: Borys Dabrowski
"""
import socket
import time
import numpy as np

class nanotec:
	"""Nanotec servo controler class"""
	def __init__(self,ip='192.168.22.100'):
		self.ip=ip

	def get(self,idx=0x6060,sub=0):
		"""Get value of the register"""
		sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		sock.connect((self.ip,80))
		sock.send(b'GET /od/%04X/%02X HTTP/1.0\r\n\r\n'%(idx,sub))
		resp=sock.recv(1024).decode()
		sock.close()

		return resp.split("\r\n")[-1].replace('"','')

	def set(self,idx=0x6060,sub=0,val="0000"):
		"""Set value of the register"""
		if isinstance(val,str): val=val.encode()
		elif np.issubdtype(type(val),np.integer): val=b'%%0%dX'%len(self.get(idx,sub))%val

		sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		sock.connect((self.ip,80))
		sock.send(b'POST /od/%04X/%02X HTTP/1.0\r\nContent-Type:'\
			b' application/x-www-form-urlencoded\r\nContent-Length:'\
			b' %i\r\n\r\n"%s"'%(idx,sub,len(val)+2,val))
		resp=sock.recv(1024).decode()
		sock.close()

		return resp.split("\r\n")[0]

	def status(self):
		"""Get status"""
		val=int(self.get(0x6041),16)
		bit=lambda x,n,mask=0x1: (x>>n)&mask

		return {
				'STATUS': val,
				'RTSO': bit(val,0),
				'SO': bit(val,1),
				'OE': bit(val,2),
				'FAULT': bit(val,3),
				'VE': bit(val,4),
				'QS': bit(val,5),
				'SOD': bit(val,6),
				'WARN': bit(val,7),
				'SYNC': bit(val,8),
				'REM': bit(val,9),
				'TARG': bit(val,10),
				'ILA': bit(val,11),
				'OMS': bit(val,12,0b11),
				'CLA': bit(val,15)
			}

	def homing(self):
		"""Perform homing of the motor"""
		self.set(0x6060,0,6)		# homing mode
		time.sleep(.1)
		self.set(0x3240,8,1)
		self.set(0x3242,3,1)
		self.set(0x6098,0,0x13)
		self.set(0x6099,1,0x50)
		self.set(0x6099,2,0x50)

		self.set(0x6040,0,0xD)		# switch on disabled
		time.sleep(.1)
		self.set(0x6040,0,0xE)		# ready to switch on
		time.sleep(.1)
		self.set(0x6040,0,0x7)		# switched on
		time.sleep(.1)
		self.set(0x6040,0,0xF)		# operation enabled
		time.sleep(.1)
		self.set(0x6040,0,0x1F)

		while ((self.status()['STATUS']>>10)&0b101)!=0b101: time.sleep(.1)

	def pos(self,p=None):
		"""Get acutal position of the motor (read encoder),
			or set an arbitrary position value (without moving)"""
		if p is not None:
			self.set(0x6063,0,p)
			time.sleep(.1)
		return int(self.get(0x6064),16)

	def run(self,pos=0,wait=False):
		"""Run motor to the new position,
			wait (or not) until position is reached"""
		self.set(0x6040,0,0xF)			# zero run bit
		self.set(0x607A,0,pos)			# set target position
		self.set(0x6040,0,0x1F)			# run

		if wait:
			time.sleep(.1)
			while not self.status()['TARG']: time.sleep(.1)

		return self.pos()

class chopper:
	def __init__(self,device='192.168.22.100',offset=0, sleeptime=None):
		"""
		Parameters:
			device (str):
				chopper device's path
			offset (int):
				**info**
		"""
		# Lock-check
		self._initialized=False
		self._device=nanotec(device)
		self._offset=offset
		d2e=lambda a: int(a*10000/360)	# degrees to encoder counts
		self._pos_array={'H':d2e(117.5),'C':d2e(66.5),'A':d2e(255),'R':d2e(0)}

	def set_cold(self):
		"""Sets the device pointing towards the cold load"""
		return self.set_pos(self._pos_array['C'])

	def set_ref(self):
		"""Sets the device pointing towards the reference source"""
		return self.set_pos(self._pos_array['R'])

	def set_hot(self):
		"""Sets the device pointing towards the hot load"""
		return self.set_pos(self._pos_array['H'])

	def set_ant(self,offset=None):
		"""Sets the device pointing towards the atmosphere"""
		if offset is None: offset=self._offset
		pos=self._pos_array['A']+offset
		return self.set_pos(pos)

	def set_pos(self,new_pos):
		"""Sets the device pointing towards selected direction

		Must be initialized already.
		"""
		if isinstance(new_pos,str): new_pos=new_pos.encode()
		assert self._initialized, "Must first initialize the chopper"

		self._device.run(new_pos)
		return 0

	def get_pos(self,tol=100):
		"""Gets the device pointing"""
		assert self._initialized, "Must first initialize the chopper"
		pos=self._device.pos()
		pr= lambda p,tol: range(self._pos_array[p]-tol,self._pos_array[p]+tol)
		if pos in pr('C',tol): return 'C'
		elif pos in pr('H',tol): return 'H'
		elif pos in pr('R',tol): return 'R'
		elif pos+self._offset in pr('A',tol): return 'A'
		else: return 'E'

	def init(self):
		"""Connection with the chopper and set the device access

		Musn't be initialized already.
		"""
		assert not self._initialized, "Cannot init initialized chopper"

		# Setup
		self._device.homing()

		self._device.set(0x6060,0,0x1)		# Postion mode
		self._device.set(0x6040,0,0xD)		# Switch on disabled
		self._device.set(0x6040,0,0xE)		# Ready to switch on
		self._device.set(0x6040,0,0x7)		# Switched on
		self._device.set(0x6040,0,0xF)		# Operation enabled

		self._device.set(0x60C5,0,0x1870)	# Setting Maximum Acceleration
		self._device.set(0x60C6,0,0x1870)	# Setting maximum Deceleration

		self._device.set(0x608F,1,10000)	# Setting encoder resolution to 10000
		self._device.set(0x608F,2,1)      	# per 1 motor revolution
		self._device.set(0x6091,1,0x7)		# Setting shaft revolutions (gearbox ratio) to 7

		self._device.set(0x6083,0,0x1870)	# Acceleration
		self._device.set(0x6084,0,0x1870)	# Deceleration
		self._device.set(0x6081,0,0x3000)	# Speed

		self._device.set(0x3210,1,0xC00)	# Proportional gain position control
		self._device.set(0x3210,2,0x0)		# Integral gain position control
		self._device.set(0x3210,3,0x2A10)	# Proportional gain velocity control
		self._device.set(0x3210,4,0x40)		# Integral gain velocity control

		# Get greetings message
		greetings=self._device.get(0x1008)+', '+self._device.get(0x100A)
		self._initialized=True
		return greetings

	def set_housekeeping(self, hk):
		""" Sets the housekeeping data dictionary.  hk must be dictionary """
		assert self._initialized, "Can set housekeeping when initialized"

		hk['Instrument']['Chopper'] = {"Position": self.get_pos()}

	def _close_and_restore(self):
		""" Close the device access"""
		assert self._initialized, "Cannot close uninitialized chopper"
		self._initialized=False

	close=_close_and_restore
