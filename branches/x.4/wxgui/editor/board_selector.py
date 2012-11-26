#!/usr/bin/env python
#-*- coding: utf-8 -*-

import wx
from wxgui._trad import _

"""-------------------------------------------------------------------------
    Selector board

    author:		Yeison Cardona
    contact:		yeison.eng@gmail.com 
    first release:	25/November/2012
    last release:	26/November/2012

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
-------------------------------------------------------------------------"""

########################################################################
class BoarSelector:
    """"""

    #----------------------------------------------------------------------
    def __init_selector__(self, IDE):
        """"""
        self.IDE = IDE


        self.buttonBoxCancel.Bind(wx.EVT_BUTTON, self.b_cancel)
        self.buttonBoxOK.Bind(wx.EVT_BUTTON, self.b_acept)
        self.radioBox_arch.Bind(wx.EVT_RADIOBOX, self.r_arch)
        self.radioBox_mode.Bind(wx.EVT_RADIOBOX, self.r_mode)

        self.IDE.loadConfig()
        self.ARCH = self.IDE.getElse("Board", "architectute", 8)
        self.MODE = self.IDE.getElse("Board", "mode", "BOOT")
        
        if self.ARCH == 8: self.radioBox_arch.SetSelection(0)
        else: self.radioBox_arch.SetSelection(1)
        
        if self.MODE == "BOOT": self.radioBox_mode.SetSelection(1)
        else: self.radioBox_mode.SetSelection(0)        

        self.new_choices_fam()
        self.new_choices_dev()


    #----------------------------------------------------------------------
    def b_acept(self, event=None):
        self.IDE.setConfig("Board", "architectute", self.ARCH)
        self.IDE.setConfig("Board", "mode", self.MODE)
        
        
        self.IDE.saveConfig()
        
        self.Close()
        
    #----------------------------------------------------------------------
    def b_cancel(self, event=None):
        """"""
        self.Close()


    #----------------------------------------------------------------------
    def r_arch(self, event=None):
        board = self.radioBox_arch.GetSelection()
        if board == 0: self.ARCH = 8
        elif board == 1: self.ARCH = 32

        self.new_choices_fam()

    #----------------------------------------------------------------------
    def r_mode(self, event=None):
        mode = self.radioBox_mode.GetSelection()
        if mode == 0: self.MODE = "ICSP"
        elif mode == 1: self.MODE = "BOOT"

        self.new_choices_fam()
        
    #----------------------------------------------------------------------
    def r_fam(self, event=None):
        sel = self.radioBox_fam.GetSelection()
        self.FAMILY = self.radioBox_famChoices[sel]

        self.new_choices_dev()

    #----------------------------------------------------------------------
    def new_choices_fam(self):
        try: self.radioBox_fam.Destroy()
        except: pass 
        columns, self.radioBox_famChoices = self.IDE.getfamilies(self.ARCH, self.MODE)
        if len(self.radioBox_famChoices) == 0:
            self.FAMILY = None
            self.new_choices_dev()
            return
	self.radioBox_fam = wx.RadioBox( self.m_panel37, wx.ID_ANY, _(u"Family"), wx.DefaultPosition, wx.DefaultSize, self.radioBox_famChoices, columns, wx.RA_SPECIFY_COLS )
	self.radioBox_fam.SetSelection( 0 )	
        self.buildChoicesFam()
        self.r_fam()
        self.new_choices_dev()


    #----------------------------------------------------------------------
    def new_choices_dev(self):
        try:
	    self.radioBox_dev.Destroy()
	    self.m_scrolledWindow1.Destroy()
        except: pass 
        columns, self.radioBox_devChoices = self.IDE.getDevices(self.ARCH, self.MODE, self.FAMILY)
        
	self.m_scrolledWindow1 = wx.ScrolledWindow( self.m_panel37, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.HSCROLL|wx.VSCROLL )
	self.m_scrolledWindow1.SetScrollRate( 5, 5 )
	self.sizer2 = wx.BoxSizer( wx.VERTICAL )
        
        self.radioBox_dev = wx.RadioBox( self.m_scrolledWindow1, wx.ID_ANY, _(u"Devices"), wx.DefaultPosition, wx.DefaultSize, self.radioBox_devChoices, columns, wx.RA_SPECIFY_COLS|wx.RA_SPECIFY_ROWS )
        self.radioBox_dev.SetSelection( 0 )
        
        self.buildChoicesDev()
        
        
        
    #----------------------------------------------------------------------
    def buildChoicesDev(self):
        self.sizer2.Add( self.radioBox_dev, 1, wx.ALL|wx.EXPAND, 5 )
	self.m_scrolledWindow1.SetSizer( self.sizer2 )
        self.m_scrolledWindow1.Layout()
        self.sizer2.Fit( self.m_scrolledWindow1 )
        self.sizer.Add( self.m_scrolledWindow1, 1, wx.EXPAND |wx.ALL, 0 )
        self.Layout()
        self.SetSize(self.Size)

        
    #----------------------------------------------------------------------
    def buildChoicesFam(self):
        self.sizer.Add( self.radioBox_fam, 0, wx.ALL|wx.EXPAND, 5 )
        self.radioBox_fam.Bind(wx.EVT_RADIOBOX, self.r_fam)