"""
Copyright (C) 2014 Andrew Skinner <obs@theandyroid.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
"""

import OBS



class ColourSquare(OBS.ImageSource):    
    def __init__(self,config):
        OBS.Log(u"__init__")
    
        self.width = 300
        self.height = 300
        self.bpp = 4
        self.pixelbuffer = bytearray(self.width*self.height*self.bpp)

        self.SetColour(0xff,0xff,255,255)
        self.CreateHotKey(0x52,self.hkey)
        self.CreateHotKey(0x47,self.hkey)
        self.CreateHotKey(0x42,self.hkey)
        self.CreateHotKey(0x57,self.hkey)
        self.CreateHotKey(0x26,self.hkey)
        self.CreateHotKey(0x28,self.hkey)
            
    def SetColour(self,r,g,b,a):
        for i in range(0,self.width*self.height*self.bpp,self.bpp):
            self.pixelbuffer[i] = b  #blue
            self.pixelbuffer[i+1] = g #green
            self.pixelbuffer[i+2] = r #red
            self.pixelbuffer[i+3] = a #alpha

    def ChangeAlpha(self,a):
        for i in range(0,self.width*self.height*self.bpp,self.bpp):
            oldAlpha = self.pixelbuffer[i+3]
            newAlpha = oldAlpha + a
            if (newAlpha > 255):
                self.pixelbuffer[i+3] = 255
            elif(newAlpha < 0):
                self.pixelbuffer[i+3] = 0 
            else:
                self.pixelbuffer[i+3] = newAlpha  
            
    def EndScene(self):
        OBS.Log(u"endScene")

    def Render(self,pos,size):
        self.DrawSprite(0xffffffff,
                        pos.x,
                        pos.y)        

    def Destructor(self):
       pass

    def hkey(self,key ,isDown):
        if(key == 0x52): #'R' key
            self.SetColour(0xff,0x00,0x00,0xff)
        elif(key == 0x47): #'G' key
            self.SetColour(0x00,0xff,0x00,0xff)
        elif(key == 0x42): #'B' key
            self.SetColour(0x00,0x00,0xff,0xff)
        elif(key == 0x57): #'W' key
            self.SetColour(0xff,0xff,0xff,0xff)
        elif(key == 0x26): #'Up Arrow' key
            self.ChangeAlpha(10)
        elif(key == 0x28): #'Down Arrow' key
            self.ChangeAlpha(-10)
            
    def GlobalSourceLeaveScene(self):
        pass
        
    def GlobalSourceEnterScene(self):
        pass

    def Tick(self,seconds):
        print (u"tick")
        pass
    
    def BeginScene(self):
        self.SetBuffers(self.pixelbuffer,u"BGRA",self.width,self.height)
        




class gui:
    def __init__(self,config):
        defaultWidth = 300
        defaultHeight = 300
        parent = config.GetParent()
        
        #you are expected to reset the width and height
        #to the render size for scaling to work after properties
        parent.SetFloat("cx",defaultWidth)
        parent.SetFloat("cy",defaultHeight)








