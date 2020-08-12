# -*- coding: utf-8 -*-
"""
Last modification: 09.07.2018

Author: Borys Dabrowski

Wobbler control library
"""

import serial,re
from time import time,sleep

class wobbler:
	"""Wobbler control library"""
	def __init__(self,device='/dev/ttyS0',baud=9600,address=b'0'):
		"""
		Parameters:
			device (str):
				Wobbler device's path
			baud (int):
				Transmission speed
			address (str):
				Address of the machine
		"""
		self._initialized=False

		# Set device
		assert len(device), "Need to have a device of length at least 1"
		self._device=device
		self._baud=baud

		# Set the address
		assert len(address)==1, "Can only have 1-long characters as address"
		if isinstance(address,str): self._address=address.encode()
		else: self._address=address

		# Device number is not initialized
		self.dev=None

		# Position initializations
		self._maxpos=40000
		self._minpos=0
		self._pos=None

	def init(self,position):
		"""Set the connection to the device

		Musn't be initialized already
		"""
		assert not self._initialized, "Cannot init initialized wobbler"
        
		# Open a serial port
		self._serial=serial.Serial(self._device,self._baud,timeout=2)

		# Set the command to Go to Initialization + position
		self._send("GI+")

		# No position is set so wait for a constant time...
		self._wait_until_motionless(2)

		# Finally, move to indicated position
		self._move(position)
		self._wait()

		self._dt = 0.0014511945645014446
		
		self._initialized=True

	def get_recommended_relative_motion(self,integration_time):
		"""
		Parameters:	
			integration_time (float or int):
				**INFO**
		"""
		return int(integration_time/self._dt/100.)*100+100

	def get_recommended_movements(self,integration_time,le=4,st=1):
		"""
		Parameters:	
			integration_time (float or int):
				**INFO**
			le (int):
				**INFO**	
			st (int):
				**INFO**
		"""
		ds=self.get_recommended_relative_motion(integration_time)
		position=self._pos*1
		if position-self._maxpos//2>0: ds*=-1

		assert not (le//2)%st, "Not cyclic"

		p=[]
		for i in range(le):
			position+=ds
			p.append(position)
			if not i % st: ds*=-1

		assert p[-1]==self._pos, "Not cyclic"
		return p

	def move(self,position):
		"""Move the device to the new position

		Must be initialized already.
		"""
		assert self._initialized, "Must initialize the wobbler before wait"
		self._move(position)

	def _move(self,position):
		assert position < self._maxpos and position > self._minpos,\
		"Wobbler position is not within max-min positions"

		# Tell the machine to Go to Absolute position
		self._pos=position
		moving=False
		while not moving:
			self._send("GA"+str(self._pos))
			self._send("PC?")
			tmp=self._read()
			moving=(tmp[1] & 0x1)

			if not moving: sleep(.1)

	def wait(self):
		"""Wait until motionless

		Must be initialized already.
		"""
		assert self._initialized, "Must initialize the wobbler before wait"
		self._wait()

	def _wait(self):
		self._wait_until_motionless()
		assert self._pos==self._query_position(), "Wobbler position not accurate"

	def _wait_until_motionless(self, time=None):
		# Get frequency of motion and return if not moving
		f=self._query_frequency()
		if not f>0: return 1

		# Continue to check untill the machine is motionless
		stat=0xFF
		while stat & 0x1:
			self._send("PC?")
			resp,stat=self._read()[0:2]

#			print("Pos:",resp)

			sleep(0.125*abs(self._pos-int(resp))/float(f) \
				if (time is None) else time)
		return 0

	def breaks(self):
		""" Send break to machine

		Must be initialized already.
		"""
		assert self._initialized, "Must initialize the wobbler before break"
		self._send("B")

	def halt(self):
		""" Send halt/stop to machine

		Must be initialized already.
		"""
		assert self._initialized, "Must first initialize the wobbler"
		self._send("H")
	stop=halt

	def query_frequency(self):
		"""Ask the machine for the frequency it is moving with

		Must be initialized already.
		"""
		assert self._initialized, "Must first initialize the wobbler"
		return self._query_frequency()

	def _query_frequency(self):
		"""Ask the machine for the frequency it is moving with"""
		self._send("PF?")
		tmp = self._read()
#		print(tmp)
		if tmp[0] == b'' or tmp[0] == '':  # ERROR: need to change return?
			return 0
		return int(tmp[0])

	def query_position(self):
		"""Ask its own position

		Must be initialized already.
		"""
		assert self._initialized, "Must first initialize the wobbler"
		return self._query_position()

	def _query_position(self):
		"""Ask the machine where it think it is"""
		self._send("PC?")
		return int(self._read()[0])

#	def print_status(self):
#		""" Ask the machine if it is moving"""
#		assert self._initialized, "Must first initialize the wobbler"
#		self._send("IS?")	# ask for extended status
#		ext_stat,stat,cs=self._read()

	def reset(self):
		""" Return the machine to its positive initialization position

		Must be initialized already.
		"""
		assert self._initialized, "Must first initialize the wobbler"

		self._send("CR")	# hardware reset
		sleep(4)
		self._wait_until_motionless()

		self._send("GI+")	# go to limit switch
		self._wait_until_motionless()

	def close(self):
		""" Close the device access

		Must be initialized to be closed.
		"""
		assert self._initialized, "Cannot close uninitialized wobbler"
		self._serial.close()
		self._initialized=False

	def send(self,cmd):
		"""Send a message
		
		Parameters:
			cmd (str):
				command to send

		Must be initialized already.
		"""
		assert self._initialized, "Must first initialize the wobbler"
		self._send(cmd)

	def _send(self,cmd):
		"""Form a telegram: <STX>AddressCommand:Checksum<ETX>"""
		if isinstance(cmd,str): cmd=cmd.encode()
		stx,etx=b'\x02',b'\x03'
		message=self._address+cmd+b':'

#		print(message)

		# calculate checksum
		checksum=0
		for n in message: checksum^=n

		# add checksum to the message
		message=message+(hex(checksum).replace('x','').encode())[-2:].upper()

		self._serial.write(stx+message+etx)

	def read(self,timeout=.5):
		"""
		Parameters:
			timeout (float):
				**INFO**

		Must be initialized already.
		"""
		assert self._initialized, "Must first initialize the wobbler"
		return self._read(timeout)
	
	def set_housekeeping(self, hk):
		""" Sets the housekeeping data dictionary.  hk must be dictionary """
		assert self._initialized, "Can set housekeeping when initialized"
		
		hk['Instrument']['Wobbler'] = {'Position': self._pos}

	def _read(self,timeout=.5):
		"""Read answer and get info from it:
			<STX>AddressStatus:Answer:Checksum<ETX>"""
		t=time()
		while self._serial.in_waiting==0 and time()-t<timeout: sleep(.1)

		if self._serial.in_waiting==0: return '',0xFF,0

		while self._serial.in_waiting>0:
			sleep(.1)
			txt=self._serial.read_all()+b''

		gr=re.search(b"\x02([^:]+):([^:]*):([^:]+)\x03$",txt).groups()
		if len(gr)==3: msg,stat,cs=gr[1],int(b'0x'+gr[0][1:],0),int(b'0x'+gr[2],0)
		else: msg,stat,cs=b'',0xFF,0

		return msg,stat,cs
