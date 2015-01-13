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
import cairo
import math
import time
from datetime import datetime,timezone

class DateTime(OBS.ImageSource):    
    def __init__(self,config):
        OBS.Log(u"__init__")
        self.config = config
        #config.SetInt("cx",1000)
        #config.SetInt("cy",100)
        self.width = 1000
        self.height = 100
        self.bpp = 4
        self.pixelbuffer = bytearray(self.width*self.height*self.bpp)
       
        
        
    def EndScene(self):
        OBS.Log(u"endScene")

    def Render(self,pos,size):
        self.ClearTexture(self.pixelbuffer)
        self.EnableBlending(True) 
        self.BlendingFunction(OBS.GS_BLEND_ONE, OBS.GS_BLEND_INVSRCALPHA, 1.0)

        surface = cairo.ImageSurface.create_for_data(self.pixelbuffer, cairo.FORMAT_ARGB32, self.width, self.height)
        ctx = cairo.Context(surface) 
                

        ctx.set_source_rgb(0, 1, 0)
        #ctx.select_font_face("Digital-7 Mono", cairo.FONT_SLANT_NORMAL,cairo.FONT_WEIGHT_NORMAL)
        
        font = cairo.FontOptions()

        current_time = datetime.now()
        stringtime = current_time.strftime('%d %b %H:%M:%S:%f')[:-3] +  " UTC"+ datetime.now(timezone.utc).astimezone().strftime('%z')

        print("size.x " + str(size.x))
        print("size.x " + str(size.y))

        #fit text to box
        for x in reversed(range(self.height)):
            #font size
            ctx.set_font_size(x)
            #rendered size
            (tx, ty, width, height, dx, dy) = ctx.text_extents(stringtime)
            if(height < self.height and width < self.width):
                ctx.set_font_size(x)
                break

        (x, y, width, height, dx, dy) = ctx.text_extents(stringtime)
        #position text
        ctx.move_to(self.width/2 - width/2, self.height/2 + height/2)    
        #draw
        
        ctx.show_text(stringtime)      
            
        surface.finish()
        
        self.DrawSprite(0xffffffff,
                        pos.x,
                        pos.y)
        self.BlendingFunction(OBS.GS_BLEND_SRCALPHA, OBS.GS_BLEND_INVSRCALPHA, 1.0)

    def Destructor(self):
       pass
            
    def GlobalSourceLeaveScene(self):
        pass
        
    def GlobalSourceEnterScene(self):
        pass

    def Tick(self,seconds):
        pass
    
    def BeginScene(self):
        self.SetBuffers(self.pixelbuffer,u"BGRA",self.width,self.height)
        name = self.config.GetParent().GetName()
        print (name)



class gui:
    def __init__(self,config):
        defaultWidth = 1000
        defaultHeight = 100
        parent = config.GetParent()
        
        #you are expected to reset the width and height
        #to the render size for scaling to work after properties
        parent.SetFloat("cx",defaultWidth)
        parent.SetFloat("cy",defaultHeight)







