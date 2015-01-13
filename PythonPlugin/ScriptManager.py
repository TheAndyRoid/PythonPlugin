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
try:
    from Tkinter import *
    from tkFileDialog import *
    from tkMessageBox import *
except ImportError:
    from tkinter import *
    from tkinter.filedialog import *
    from tkinter.messagebox import *
try:
    from ttk import Frame, Button, Label, Style, Combobox, LabelFrame
except ImportError:
    from tkinter.ttk import Frame, Button, Label, Style, Combobox, LabelFrame

#py unicode problems
if sys.version_info < (3,0):
    import codecs
    def u(x):
        return unicode(x,"utf-8")
else:
    def u(x):
        return x


import ctypes,ast
import ctypes.wintypes


import os.path
import OBS

import json
import subprocess


def loadJSON(path):
        scriptDict = {}
        #check if file exists
        if(os.path.isfile(path)):
            file = open(path)
            scriptDict = json.loads(json.load(file))
            file.close()

        print (scriptDict)
        return scriptDict

def saveJSON(scriptDict,path):
    with open(path, 'w') as outfile:
        json.dump(json.dumps(scriptDict,sort_keys=True,indent=4, separators=(',', ': ')), outfile)
        

def createScriptDict():
    d = {}
    d["debug"] = 0
    d["guiClass"]  = None
    d["file"]  = None
    d["mainClass"]  = None
    return d

 
def getScriptNames(d):
        names = []
        for key in d.keys():
            names.append(key)
        return names

def saveSourceSettings(d,config):
        config.SetString(u"PythonMainFile",u(d["mainFile"]))
        config.SetString(u"PythonMainClass",u(d["mainClass"]))
        config.SetString(u"PythonGUIClass", u(d["guiClass"]))
        config.SetInt(u"Debug",d["debug"])

def setColumnWeight(element,colNum):
    for i in range(colNum):
        element.columnconfigure(i,weight=1)
        element.rowconfigure(i,weight=1)
        

def createSettings(self,frame,d):


    #name frame
    row = 0
    col = 0

    
    lblFrame = Frame(frame)
    lblFrame.grid(row=row,column=col,columnspan=5,sticky=W+E,pady=0,padx=5)

    frame.columnconfigure(col, weight=1)
    frame.rowconfigure(row, weight=0)

    row = 0
    col = 0

    Label(lblFrame, text="Script Name").grid(row=row,column=col,sticky=W) 
    lblFrame.columnconfigure(col, weight=0)
    lblFrame.rowconfigure(row, weight=0)
    

    row = 0
    col = 1
    
    entryMain = Entry(lblFrame)
    entryMain.grid(row=row,column=col,sticky=W+E,columnspan=5)
    self.entryName = entryMain
    lblFrame.columnconfigure(col, weight=1)
    lblFrame.rowconfigure(row, weight=0)


    #python plugin script file
    row = 1
    col = 0
    mainGroup = LabelFrame(frame,text="Python Script File", padding="5 5 5 5")
    mainGroup.grid(row=row,column=col,columnspan=5,sticky=W+E+N,pady=5,padx=5)
          


    row = 0
    col = 0
    mainGroup.columnconfigure(col, weight=1)
    mainGroup.rowconfigure(row, weight=0)  
    
    entryMain = Entry(mainGroup)
    entryMain.grid(row=row,column=col,columnspan=4,sticky=W+E)
    entryMain.bind("<Return>",self.entryFileEvent)
    self.entryFile = entryMain


    row = 0
    col = 4
    btnMainBrowse = Button(mainGroup,text="Browse")
    btnMainBrowse.grid(row=row,column=col,columnspan=1,sticky=W+E,padx=5,pady=2.5)
    self.btnBrowse = btnMainBrowse

    
    row = 1
    col = 0   
    Label(mainGroup, text="Plugin Class").grid(row=row,column=col,sticky=E)
    self.cbPluginClass = comboBox([],mainGroup,col+1,row)
    mainGroup.columnconfigure(col, weight=0)
    row = 2
    col = 0   
    Label(mainGroup, text="GUI Class").grid(row=row,column=col,sticky=E)
    self.cbGuiClass = comboBox([],mainGroup,col+1,row)

    col = 1
    mainGroup.columnconfigure(1, weight=1)
    mainGroup.columnconfigure(2, weight=1)
    mainGroup.columnconfigure(3, weight=1)  
   

    row=1
    col=4
    
    button = Button(mainGroup,text="Edit Script File")
    button.grid(row=row,column=col,sticky=W+E,padx=5,pady=2.5)
    self.btnEditScript = button

    row = 2
    col = 4
    button = Button(mainGroup,text="Open Contaning Folder")
    button.grid(row=row,column=col,sticky=W+E,padx=5,pady=2.5)
    self.btnOpenFolder = button

 

def comboBox(classList,frame,col,row):
    combobox = Combobox(frame,state='readonly')
    combobox.grid(row=row,column=col,sticky=W)
    return combobox

def listboxReload(listbox,items):
    listbox.delete(0,END)
    for item in items:
        listbox.insert(END,item)
    




class ScriptManager:
    def __init__(self,sourceData):
        self.sourceData = sourceData


        self.jsonPath = OBS.GetAppDataPath()+"/pluginData/Python/scripts.json"        
        self.scriptDict = loadJSON(self.jsonPath)
        self.currentDict = []
        self.selectedScript = None
        
        print(getScriptNames(self.scriptDict))

        self.initUI()

    def initUI(self):
        self.width = 700
        self.height = 350
        self.left = 0
        self.right = 0
        
        
        self.parent = Tk()
        self.parent.attributes("-topmost", 1)
        
        self.getHWNDS()
        r = self.winPos(self.obshwnd)
        self.centerOverOBS(r)

        self.parent.geometry("%sx%s+%s+%s" % (self.width,self.height,int(self.left),int(self.top)))
        
        self.parent.focus()
        self.parent.wm_title("Python Script Manager")
       

        #title label frame
        row = 0
        col = 0
        self.parent.columnconfigure(col,weight=1) 
        self.parent.rowconfigure(row,weight=0)
        
        titleFrame = Frame(self.parent)   
        titleFrame.grid(row=row,column=col,sticky=W+E,pady=5,padx=10)      

        row = 0
        col = 0
        labelfont = ('bold', 15)
        Label(titleFrame, text="Python Script Manager",font=labelfont).grid(row=row,column=col,sticky=W)

        titleFrame.columnconfigure(row,weight=1)
        titleFrame.rowconfigure(col,weight=0)
        
       
        #sidebyside frame
        row = 1
        col = 0
        self.parent.rowconfigure(row,weight=1)
        self.parent.columnconfigure(col,weight=1)
        
        sidebyside = Frame(self.parent)
        sidebyside.grid(row=row,column=col,sticky=W+E+N+S,pady=0,padx=5)

        
        
        #listbox frame
        row = 0
        col = 0
        sidebyside.rowconfigure(row,weight=1)
        sidebyside.columnconfigure(col,weight=1)        
        
        leftFrame = Frame(sidebyside)
        leftFrame.grid(row=row,column=col,sticky=W+E+N+S,pady=5,padx=5)

        #listbox
        row = 0
        col = 0
        self.listbox = Listbox(leftFrame)
        self.listbox.grid(row=row,column=col,columnspan=5,sticky=W+E+N+S)
        listboxReload(self.listbox,getScriptNames(self.scriptDict))
        self.listbox.bind('<<ListboxSelect>>',self.listboxSelectionEevent)

        leftFrame.columnconfigure(col,weight=1)
        leftFrame.rowconfigure(row,weight=1)

        

        row=1
        col=0
        buttonFrame = Frame(leftFrame)   
        buttonFrame.grid(row=row,column=col,sticky=W,pady=0,padx=0)

        row=0
        col=0
        #btnAdd
        button = Button(buttonFrame,text="Add")
        button.grid(row=row,column=col,sticky=E)        
        self.btnAdd = button
        self.btnAdd.config(command=self.btnAddEvent)

        #btnRemove
        button = Button(buttonFrame,text="Remove")
        button.grid(row=row,column=col+1,sticky=W)
        self.btnRemove = button
        self.btnRemove.config(command=self.btnRemoveEvent)
        
       
        #settings frame

        row = 0
        col = 1
        sidebyside.rowconfigure(row,weight=1)
        sidebyside.columnconfigure(col,weight=10)


        rightFrame = Frame(sidebyside)
        rightFrame.grid(row=row,column=col,sticky=W+E+N+S,pady=5,padx=5)

        

        row = 0
        col = 0
        rightFrame.columnconfigure(col,weight=1)
        rightFrame.rowconfigure(row,weight=1)
                
        self.frmSettings = LabelFrame(rightFrame,text="Settings", padding="5 5 5 5")
        self.frmSettings.grid(row=row,column=col,sticky=W+E+N+S,pady=0,padx=0) 
        
 
        self.frmSettings.columnconfigure(col,weight=1)
        self.frmSettings.rowconfigure(row,weight=1)


        d = createScriptDict()
        createSettings(self,self.frmSettings,d)


        row=1
        col=0
        
        buttonFrame = Frame(rightFrame)   
        buttonFrame.grid(row=row,column=col,sticky=W,pady=0,padx=0)        
        
    
        row = 0
        col = 0
        button = Button(buttonFrame,text="Save")
        button.grid(row=row,column=col)
        self.btnSave = button
        self.btnSave.config(command=self.btnSaveEvent)
    


    








        #ok cancel buttons
        row = 2
        col = 0
        self.parent.columnconfigure(col,weight=1)
        self.parent.rowconfigure(row,weight=0)
        
        bottomframe = Frame(self.parent)   
        bottomframe.grid(row=row,column=col,sticky=W+E,pady=5,padx=10)

        row = 0
        col = 0
        
        bottomframe.columnconfigure(col,weight=1)
        bottomframe.rowconfigure(row,weight=0)
        
        logFrame = Frame(bottomframe)   
        logFrame.grid(row=row,column=col,sticky=W,pady=0,padx=0)
        logFrame.rowconfigure(row,weight=0)

        row=0
        col=0
        #btnOpenLogs
        button = Button(logFrame,text="Open Logs",command=self.btnOpenLogsEvent)
        button.grid(row=row,column=col,sticky=E,padx=0,pady=0)        
        self.btnOpenLogs = button
 

        row = 0
        col = 1
        

        bottomframe.columnconfigure(col,weight=1)
        bottomframe.rowconfigure(row,weight=0)

        ccFrame = Frame(bottomframe)   
        ccFrame.grid(row=row,column=col,sticky=E,pady=0,padx=5)
        ccFrame.rowconfigure(row,weight=0)

        row=0
        col=0
        
        button = Button(ccFrame,text="Create",command=self.btnCreateEvent)
        button.grid(row=row,column=col,sticky=E,padx=10,pady=0)
        self.btnCreate = button

        button = Button(ccFrame,text="Cancel",command=self.btnCancelEvent)
        button.grid(row=row,column=col+1,sticky=W,padx=0,pady=0) 
        self.btnCancel = button
       

        
        self.clearSettings()
        self.parent.after(1,self.lockWindow)
        self.parent.mainloop()

        self.unlockWindow()



    def btnAddEvent(self):
        self.clearSettings()
        
    def btnSaveEvent(self):
        name = self.entryName.get()        
        print (name)
        self.saveSettings(name)


    def getHWNDS(self):
        self.obshwnd = OBS.GetMainWindow()
        self.hwndTkinter =  self.parent.winfo_id()

    def centerWindow(self):
        self.winPos(self.parent.winfo_id())
    

    def winPos(self,hwnd):
        f = ctypes.windll.user32.GetWindowRect
        rect = ctypes.wintypes.RECT()
        DWMWA_EXTENDED_FRAME_BOUNDS = 9
        f(ctypes.wintypes.HWND(hwnd),
          ctypes.byref(rect)
          )
        return rect
    
    def centerOverOBS(self,r):
        obsWidth = r.right-r.left
        obsHeight = r.bottom-r.top
        self.left = r.left + (obsWidth/2) - (self.width/2)
        self.top = r.top + (obsHeight/2) - (self.height/2)
        

    def lockWindow(self):
        #ctypes.windll.user32.SetWindowLongA(self.hwndTkinter,-8,self.obshwnd)
        #ctypes.windll.user32.SetParent(self.hwndTkinter, self.obshwnd)
            
        ctypes.windll.user32.SetActiveWindow(self.hwndTkinter)
        ctypes.windll.user32.EnableWindow(self.obshwnd,False)
        

    def unlockWindow(self):        
        ctypes.windll.user32.EnableWindow(self.obshwnd,True)
        ctypes.windll.user32.SetActiveWindow(self.obshwnd)


    def listboxSelectionEevent(self,evt):
        self.clearSettings()
        print("Select")
        w = evt.widget
        index = int(w.curselection()[0])
        value = w.get(index)
        d = self.scriptDict[value]
        self.loadSettings(d,value)
        

    def loadSettings(self,d,name):
        self.clearSettings()
        self.entryName.insert(0,name)
        self.entryFile.insert(0,d['file'])
        file = d['file']
        print ("osetuh"+ file)
        if file is not None:
            classes = getClasses(d['file'])
            print (classes)
            if  not classes:
                self.cbPluginClass['values']= ['']
                self.cbGuiClass['values']= ['']
                self.cbPluginClass.current(0)
                self.cbGuiClass.current(0)
            else:
                print (classes.index(d['mainClass']))
                self.cbPluginClass['values']= classes
                self.cbPluginClass.current(classes.index(d['mainClass']))
                self.cbGuiClass['values']= classes
                self.cbGuiClass.current(classes.index(d['guiClass']))
        else:
            file= OBS.GetAppDataPath()+"/pluginData/Python"
           
        print ("LoadSettings" + file)
        self.btnBrowse.config(command=lambda arg=os.path.dirname(file):self.btnBrowseEvent(arg))
        self.btnOpenFolder.config(command=lambda arg=os.path.dirname(file): openContaningFolder(arg))
        self.btnEditScript.config(command=lambda arg=file: openFileWithDefaultOSApp(arg))
        
    def clearSettings(self):
        self.entryName.delete(0,END) 
        self.entryFile.delete(0,END)
        self.btnBrowse.config(command=None)
        self.cbPluginClass['values'] = ['']
        self.cbGuiClass['values'] = ['']
        self.cbPluginClass.current(0)
        self.cbGuiClass.current(0)
        self.btnEditScript.config(command=None)
        self.btnOpenFolder.config(command=None)
        self.btnBrowse.config(command=lambda file= OBS.GetAppDataPath()+"/pluginData/Python" :self.btnBrowseEvent(file))

    def saveSettings(self,name):

        #check if dict exists
        d = self.scriptDict.get(name)
        if d is None:
            d= createScriptDict()

        d['guiClass']= self.cbGuiClass.get()
        d['file'] = self.entryFile.get()
        d['mainClass']= self.cbPluginClass.get()
        if validateSettings(d['file'],name,d['guiClass'],d['mainClass']) == False:
            self.listbox.focus_set()
            return True
        #add to dict
        self.scriptDict[name] = d
        print (d)
        #reload listbox
        listboxReload(self.listbox,getScriptNames(self.scriptDict))
        #save to json
        saveJSON(self.scriptDict,self.jsonPath)

    def updateClasses(self,file):
        classes = getClasses(file)        
        if  not classes:
            self.cbPluginClass['values']= ['']
            self.cbGuiClass['values']= ['']
            self.cbPluginClass.current(0)
            self.cbGuiClass.current(0)
        else:
            self.cbPluginClass['values']= classes
            self.cbPluginClass.current(0)
            self.cbGuiClass['values']= classes
            self.cbGuiClass.current(0)

    def btnRemoveEvent(self):
        #remove the selected item from list box.
        w = self.listbox
        try:
            index = int(w.curselection()[0])
            value = w.get(index)
            del self.scriptDict[value]
            self.clearSettings()
            listboxReload(self.listbox,getScriptNames(self.scriptDict))
        except:
            pass
        
    def entryFileEvent(self,event):
        path = self.entryFile.get()
        self.updateComboBoxes(path)

    def btnBrowseEvent(self,file):
        print ("btnbrowsefile " + file)
        if file is None:
            #path = askopenfilename(parent=self.parent,initialdir=("C:\\"))
            path = askopenfilename(parent=self.parent,initialdir=(OBS.GetAppDataPath()+"/pluginData/Python"))
        else:
            path = askopenfilename(parent=self.parent,initialdir=(file))
        if path == '':
            #user cancled do nothing.
            return
        self.updateComboBoxes(path)
            
    def updateComboBoxes(self,path):
        #need to update comboboxes, edit and open contatining folder.
        self.updateClasses(path)
        self.entryFile.delete(0,END)
        self.entryFile.insert(0,path)

        self.btnOpenFolder.config(command=lambda arg=os.path.dirname(path): openContaningFolder(arg))
        self.btnEditScript.config(command=lambda arg=path: openFileWithDefaultOSApp(arg))

    def btnCancelEvent(self):
        self.parent.destroy()

    
    def btnCreateEvent(self):
        file = self.entryFile.get()
        pluginClass = self.cbPluginClass.get()
        guiClass = self.cbGuiClass.get()
        name = " "
        if validateSettings(file,name,guiClass,pluginClass) == True:
            self.sourceData.SetString(u"PythonFile",u(file))
            self.sourceData.SetString(u"PythonPluginClass",u(pluginClass))
            self.sourceData.SetString(u"PythonGUIClass",u(guiClass))
            self.sourceData.SetInt(u"Debug",0)
            self.btnCancelEvent()
    
    def btnOpenLogsEvent(self):
        stdErr = OBS.GetAppDataPath()+"/pluginData/Python/stdErr.txt"        
        stdOut = OBS.GetAppDataPath()+"/pluginData/Python/stdOut.txt"        
        openFileWithDefaultOSApp(stdErr)
        openFileWithDefaultOSApp(stdOut)

def getClasses(file):
    try:
        ofile = open(file,'r')
        text = ofile.read()
        p = ast.parse(text)
        classes =  [ node.name for node in ast.walk(p) if isinstance(node, ast.ClassDef)]
        ofile.close()
    except IOError as e:
        myWarn("File Error","I/O error({0}): {1}".format(e.errno, e.strerror))
        classes = [] 
    except SyntaxError as e:
        print (e)
        e.filename = file
        myWarn(file,e )
        classes = []    
    except:
        classes = [] 
        print ("Unknown Problem Loading File")
        
    return classes

def openContaningFolder(folder):
    #convert to windows path format
    folder = folder.replace("/","\\")
    print (folder)
    cmd = ['c:\windows\EXPLORER.EXE', '/n,%s' % folder]
    subprocess.Popen(cmd)

def openFileWithDefaultOSApp(file):
    os.startfile(file)



def validateSettings(file,name,guiClass,pluginClass):

    if name == '':
        myWarn("Blank Name","Blank name!\n Cannot Save")
        return False

    if file == '':
        myWarn("No File","No python file selected!")
        return False    
    

    if pluginClass == '':
        myWarn("No class","No class selected for Plugin class!")
        return False

    if guiClass == '':
        myWarn("No class","No class selected for GUI class!")
        return False

    if guiClass == pluginClass:
        myWarn("Identical classes","Plugin Class and GUI Class cannot be the same!\n")
        return False
    
    

    return True

def myWarn(title,text):
    if sys.version_info < (3,0):
        showwarning(title,text)
    else:
        showwarning(title,text)
     









    


 






    
