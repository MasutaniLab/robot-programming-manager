#!/usr/bin/env python
# -*- coding: utf8 -*-

import Tkinter as TK
import tkFileDialog
import ttk
import sys
import subprocess
import socket
import os
import tkMessageBox
import time
import rtctree.tree
import re #正規表現
import yaml
import shutil
from collections import OrderedDict
yaml.add_constructor(yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG,
    lambda loader, node: OrderedDict(loader.construct_pairs(node)))


#グローバル変数群
data=[]
pyComp=[]

commonComp=[]
commonexe=[]

r_root=""
r_localexe=[]
r_localcomp=[]
r_remotecomp=[]
r_rtcon=[]

s_root=""
s_simexe={}
s_label=[]
simexe=[]
s_remoteexe=[]
simComp=[]
s_localexe=[]
s_localcomp=[]
s_remotecomp=[]
s_rtcon=[]

root1=None
confnum=""
confButton=[]
confName=[]
confVal=[]
decideButton=""

HelpMessage = None
HostCxt = socket.gethostname() + ".host_cxt"

Tree = rtctree.tree.RTCTree()
conError=0

###OSの判断
OS = ''
if sys.platform.startswith('win'):
    OS = 'windows'
elif sys.platform.startswith('linux'):
    OS = 'linux'
else:
    print(sys.platform + 'はサポートしていません');
    sys.exit()
print OS
###共通の関数
def ping(arg):
    if OS=="windows":
        proc = subprocess.Popen("ping " + arg + " -n 1", shell=True, stdout=subprocess.PIPE)
        for line in proc.stdout:
            if "TTL=" in line:
                return True
        return False
    elif OS=="linux":
        proc = subprocess.Popen("ping " + arg + " -c 1", shell=True, stdout=subprocess.PIPE)
        for line in proc.stdout:
            if "ttl=" in line:
                return True
        return False

def Message(arg):
    global HelpMessage
    HelpMessage["text"] = arg

def findProcess(arg):
########   Windows   ########
    if OS == 'windows':
        if arg.find(".py")!=-1:
            arg="python "+arg
            arg2 = arg.replace(" ", "  ", 1) #wmicで得られるコマンドラインの内容は最初の空白に空白が追加されるので
            proc = subprocess.Popen("wmic process get processid,commandline /format:csv", shell=True, stdout=subprocess.PIPE)
            for line in proc.stdout:
                if arg2 in line:
                    #print line
                    return True
            return False
        exeName = re.sub(r'^.*(\\|/)(.*\.exe).*$','\\2', arg) #xxx.exeの部分だけを抜き出す
        proc = subprocess.Popen("tasklist", shell=True, stdout=subprocess.PIPE)
        for line in proc.stdout:
            if exeName in line:
                return True
        return False
########   Linux   ########
    elif OS == 'linux':
        exeName = re.sub(r'^(.*)/(.*)(\-).*$','\\2', arg)
        proc = subprocess.Popen("ps aux", shell=True, stdout=subprocess.PIPE)
        for line in proc.stdout:
            if exeName in line:
                return True
        return False

def startExe(arg):
########   Windows   ########
    if OS =='windows':
        if arg.find(".py")!=-1:
            if findProcess(arg):
                tkMessageBox.showerror('Error', arg + "は既にあります")
                raise SystemError
            else:
                subprocess.Popen("python  " + arg,shell=True)
                print "start : "+arg
                return
        exeName = re.sub(r'^.*(\\|/)(.*\.exe).*$','\\2', arg) #xxx.exeの部分だけを抜き出す
        print "start : "+exeName
        if findProcess(arg):
            tkMessageBox.showerror('Error', exeName + "は既にあります")
            raise SystemError
        arg1 = "start "+'\"'+exeName+'\" '+arg
        #print arg1
        subprocess.call(arg1,shell=True)
########   Linux   ########
    elif OS == 'linux':
        if arg.find(".py")!=-1:
            if findProcess(arg):
                tkMessageBox.showerror('Error', arg + "は既にあります")
                raise SystemError
            else:
                arg1= "gnome-terminal -e " + "\"python "+ arg+ "\""
                #arg1= "python "+ arg
                subprocess.Popen(arg1,shell=True)
                return
        exeName = re.sub(r'^.*(\\|/)(.*\ ).*$','\\2', arg)
        if findProcess(arg):
            tkMessageBox.showerror('Error', exeName + "は既にあります")
            raise SystemError
        arg1 = "gnome-terminal -e " + "\""+ arg+ "\""
        print "start : "+exeName
        subprocess.call(arg1,shell=True)

def finishExe(arg):
########   Windows   ########
    if OS == 'windows':
        if arg.find(".py")!=-1:
            arg="python "+arg
            arg2 = arg.replace(" ", "  ", 1) #wmicで得られるコマンドラインの内容は最初の空白に空白が追加されるので
            proc = subprocess.Popen("wmic process get processid,commandline /format:csv", shell=True, stdout=subprocess.PIPE)
            for line in proc.stdout:
                if arg2 in line:
                    pid = re.sub(r'^.*,(.*)$', '\\1', line)
                    #print("Killing "+pid+"...")
                    subprocess.call("taskkill /f /pid "+pid, shell=True)
            print "finish : "+arg
        else:
            exeName = re.sub(r'^.*(\\|/)(.*\.exe).*$','\\2', arg) #xxx.exeの部分だけを抜き出す
            print "finish : "+exeName
            subprocess.call("taskkill /F /im " + exeName, shell=True)
########   Linux   ########
    elif OS == 'linux':
        if arg.find(".py")!=-1:
            proc=subprocess.Popen("ps w |grep " +"\""+arg+"\"",shell=True,stdout=subprocess.PIPE)
            for j in proc.stdout:
                pid=j[:(j.find("pts")-1)]
                subprocess.call("kill "+pid,shell=True)
                break
            print "finish : "+arg
        else:
            exeName = re.sub(r'^.*(\\|/)(.*\ )(\-).*$','\\2', arg)
            subprocess.call("killall " + exeName, shell=True)
            print "finish : "+exeName

############
###RTC関係
def rtcConnect(comp1, port1, comp2, port2):
    c1 = Tree.get_node(comp1)
    p1 = c1.get_port_by_name(port1)
    c2 = Tree.get_node(comp2)
    p2 = c2.get_port_by_name(port2)
    p1.connect(dests=[p2])

def rtcExit(comp):
    try:
        c = Tree.get_node(comp)
        c.exit()
    except:
        print "Exit Fail"

def rtcDisconnectAll(comp):
    c = Tree.get_node(comp)
    try:
        c.disconnect_all()
    except:
        print "disconnect Fail"

def rtcState(comp):
    try:
        c = Tree.get_node(comp)
        return c.state_in_ec(0)
    except:
        print "State Fail,Maybe Zombie"
        return 1
def rtcIsComponent(comp):
    return Tree.is_component(comp)

def rtcActivate(comp):
    c = Tree.get_node(comp)
    c.activate_in_ec(0)

def rtcDeactivate(comp):
    try:
        c = Tree.get_node(comp)
        c.deactivate_in_ec(0)
    except:
        print "Deactive Fail"
def rtcReset(comp):
    c = Tree.get_node(comp)
    c.reset_in_ec(0)

def rtcRTcon(comps,rtcon):
    for i in range(0,len(rtcon),+1):
         e=rtcon[i].find(" ")
         Out=rtcon[i][:e]
         In=rtcon[i][e+1:]
         #RTconをoutとinに分ける
         ocomp="1"
         oport="2"
         icomp="3"
         iport="4"
         for j in range(0,len(comps),+1):
             #out側
             comp=str(comps[j])
             op=Out.find(":")
             outrtc=Out[:op]
             if comp.find(outrtc) != -1:
                 #get_componentのコンポーネントとRTconのコンポーネントが同じ場合
                 ocomp=comps[j]
                 oport=Out[op+1:]
                 for j in range(0,len(comps),+1):
                     #in側
                     comp=str(comps[j])
                     ip=In.find(":")
                     inrtc=In[:ip]
                     if comp.find(inrtc) !=-1:
                         #get_componentのコンポーネントとRTconのコンポーネントが同じ場合
                         icomp=comps[j]
                         iport=In[ip+1:]
                 global conError
                 try:
                     rtcConnect(ocomp,oport,icomp,iport)
                 except:
                     print conError
                     if conError==0:
                         tkMessageBox.showerror('Error',"接続失敗\nもう一度試してください")
                         conError+=1
                         return False
                     elif conError >= 1:
                         tkMessageBox.showerror('Error',"接続失敗\n設定ファイルの記述ミス- rtc : の部分を確認してください")
                         return False

    return True

###シミュレーション用の関数
def sRTconnect():
    print "USER : "+HostCxt

    Tree.add_name_server(s_root)
    Tree.get_node(['/', s_root]).reparse()
    srcomp=[]
    for s_comp in s_remotecomp:
        srcomp.append(s_comp)
    for s_comp in s_localcomp:
        srcomp.append(s_comp)
    stop = 0
    while 1:
        compall=False
        compOK=[]
        for i in srcomp:
            compOK.append(rtcIsComponent(i))
            if len(compOK) == len(srcomp):
                for ok in compOK:
                    if ok == True:
                        compall=True
                    else:
                        compall=False
                        break
        if compall:
            print "confirm"
            #time.sleep(1)
            Tree.get_node(['/', s_root]).reparse()
            break
        else:
            stop+=1
            if stop > 42:
                tkMessageBox.showerror('Error',"もう一度実行するか\n設定ファイルの- rtc: に関する部分を確認してください")
                return
            print "yet"
            time.sleep(0.13)
            Tree.get_node(['/', s_root]).reparse()


###
    conError=0
    if not rtcRTcon(srcomp,s_rtcon):
        return
    tkMessageBox.showinfo('Notification',"接続確認\nシミュレーション開始できます")
    #confResetting(s_root)
    print pyComp
    for i in pyComp:
        rtcActivate(i)
            
def sRTexit(event):
    #ボタン押し間違い処理
    try:
        Tree.get_node(['/', s_root]).reparse()
    except:
        try:
            Tree.add_name_server(s_root)
        except:
            tkMessageBox.showerror('Emergency',"ネームサーバを確認できません")
            return   
    for i in s_localcomp:
        if  not rtcIsComponent(i):
            tkMessageBox.showerror('Emergency',"ユーザRTCが起動していません")
            return
    #ディアクティベート(pythonRTC)
    for i in pyComp:
        rtcDeactivate(i)
        
    scomp=[]
    lcomp=[]
    for i in s_remotecomp:
        scomp.append(i)
    for i in s_localcomp:
        lcomp.append(i)

    #ディアクティベートを促す
    for i in lcomp:
        m = rtcState(i)
    if m!=1 :
        tkMessageBox.showerror('Error',"ユーザRTC動作中\nシミュレーションを停止してください")
        return
    #実際にディアクティベート
    else:
        for i in scomp:
            rtcDisconnectAll(i)
        for i in lcomp:
            rtcExit(i)
    time.sleep(2)
    for j in s_localexe:
        finishExe(j)
    confdestroy()


def startSimulator(arg):
    #実機側を操作していないことを確認
    for i in r_localexe:
        if findProcess(i):
            tkMessageBox.showerror('Error', "実機のユーザRTCを先に終了してください")
            return
    if OS == 'windows':
        simName = "choreonoid.exe"
    elif OS == 'linux':
        simName = "choreonoid"
    if  findProcess(simName):
        tkMessageBox.showerror('Error', "コレオノイドがすでに起動しています")
        return
    #コンボボックスに対応したプロジェクトファイルで起動
    try:
        for key,value in s_simexe.iteritems():
            tag = key.find(arg)
            if tag != -1:
                startExe(s_simexe[key])
                break
        for i in s_remoteexe:
            startExe(i)
    except:
        tkMessageBox.showerror('Error', "支援RTCを起動できませんでした\nビルドや設定ファイルを確認してください")
        return 

def sStartController(event):
    if OS == 'windows':
        simName = "choreonoid.exe"
    elif OS == 'linux':
        simName = "choreonoid"
    #エラー処理
    if not findProcess(simName):
        tkMessageBox.showerror('Error', "コレオノイドを先に起動してください")
        return
    try:
        Tree.add_name_server(s_root)
        Tree.get_node(['/', s_root]).reparse()
    except:
        tkMessageBox.showerror('Error', "choreonoidでネームサーバがうまく動作していません\nChoreonoidでシミュレーションを一瞬実行してみてください")
        return
    #ユーザRTC起動
    try:
        for i in s_localexe:
            startExe(i)
    except:
        tkMessageBox.showerror('Error', "ユーザRTCを起動できませんでした\nビルドや設定ファイルを確認してください")
        return 
    sRTconnect()

def finishSimulator(event):

    for i in s_localexe:
        if findProcess(i):
            tkMessageBox.showerror('Error', "ユーザRTCを先に終了してください")
            return
        
    if OS == 'windows':
        simName = "choreonoid.exe"
        cname = "cnoid-nameserver.exe"
    elif OS == 'linux':
        simName = "choreonoid"
        cname ="cnoid-nameserver"
    #ボタン押し間違い処理
    if not findProcess(simName):
        tkMessageBox.showerror('Error', "コレオノイドが存在しません")
        return
    for i in s_remoteexe:
        if not findProcess(i):
            tkMessageBox.showerror('Error', "支援RTCが存在しません")
        else:
            finishExe(i)
    finishExe(cname)
    finishExe(simName)


###実機用の関数
def rStartController(event):
    if OS == 'windows':
        simName = "choreonoid.exe"
    elif OS == 'linux':
        simName = "choreonoid"
    if findProcess(simName):
        tkMessageBox.showerror('Error', "コレオノイドを終了してください")
        return
    if not ping(r_root):
        tkMessageBox.showerror('Error',"搭載PCと通信できません")
        return
    else:
        print "ping : OK"
    try:
        Tree.add_name_server(r_root)
        print "RemotePC : OK"
    except:
        tkMessageBox.showerror('Error',"搭載PCの情報を取得できません")
        return
    for i in r_remotecomp:
        if not rtcIsComponent(i):
            tkMessageBox.showerror('Error',"搭載PC側の"+i[len(i)-1]+"コンポーネントが足りていません")
            return
        elif rtcState(i) != 1:
            tkMessageBox.showerror('Error', "搭載PC側の"+i[len(i)-1]+"コンポーネントが待機中でありません")
            return
   
    #他のマシンのControllerコンポーネントが存在しないことを確認
    for i in r_localcomp:
        rtc=i[-1]
        strs = Tree.iterate(lambda n, a: n.full_path_str,filter=['is_component', '_name=="'+rtc+'"'])
        for i in strs:
            print i
            if i.find("localhost")!=-1:
                continue
            elif len(i) != 0:
                tkMessageBox.showerror('Error',"以下のユーザRTCが存在しています\n"+i)
                return

    for i in r_localexe:
        try:
            startExe(i)
        except:
            print("Exception")
            return
    rRtconnect()

##
def rRtconnect():
    Tree.get_node(['/', r_root]).reparse()
    srcomp=[]
    for i in r_remotecomp:
        srcomp.append(i)
    for i in r_localcomp:
        srcomp.append(i)
    stop=0
    while 1:
        compall=False
        compOK=[]
        for i in srcomp:
            compOK.append(rtcIsComponent(i))
            if len(compOK) == len(srcomp):
                for ok in compOK:
                    if ok == True:
                        compall=True
                    else:
                        compall=False
                        break
        if compall:
            print "confirm"
            #time.sleep(1)
            Tree.get_node(['/', r_root]).reparse()
            break
        else:
            stop+=1
            if stop > 42:
                tkMessageBox.showerror('Error',"もう一度実行するか\n設定ファイルの- rtc: に関する部分を確認してください")
                return
            print "yet"
            time.sleep(0.13)
            Tree.get_node(['/', r_root]).reparse()

    conError=0
    if not rtcRTcon(srcomp,r_rtcon):
        return
    tkMessageBox.showinfo('Notification',"接続確認\nアクティベートできます")
    #confResetting(r_root)

def rActivate(event):
    #ボタン押し間違い処理
    try:
        Tree.get_node(['/', r_root]).reparse()
    except:
        try:
            Tree.add_name_server(r_root)
        except:
            tkMessageBox.showerror('Emergency',"ネームサーバを確認できません")
            return        
    for i in r_localcomp:
        if  not rtcIsComponent(i):
            tkMessageBox.showerror('Emergency',"ユーザRTCが起動していません")
            return
    for i in r_remotecomp:
        if rtcState(i) != 1:
            tkMessageBox.showerror('Error', "搭載PC側の"+i[len(i)-1]+"コンポーネントが待機中でありません")
            return
    #アクティベート
    for i in r_remotecomp:
        rtcActivate(i)
    for i in r_localcomp:
        rtcActivate(i)

def rDeactivate(event):
    #ボタン押し間違い処理
    try:
        Tree.get_node(['/', r_root]).reparse()
    except:
        try:
            Tree.add_name_server(r_root)
        except:
            tkMessageBox.showerror('Emergency',"ネームサーバを確認できません")
            return
    for i,j in zip(r_localcomp,r_remotecomp):
        if  not rtcIsComponent(i):
            tkMessageBox.showerror('Emergency',"ユーザRTCが起動していません")
            return
        elif rtcState(i) == 1:
            tkMessageBox.showwarning('Notification',"ユーザRTCがアクティベート中ではりません")
        if  not rtcIsComponent(j):
            tkMessageBox.showerror('Emergency',"サポートRTCが起動していません")
            return
        elif rtcState(j) == 1:
            tkMessageBox.showwarning('Notification',"サポ－とRTCがアクティベート中ではりません")
    #ディアクティベート，エラーの場合はリセット
    for i in r_localcomp:
        rtcDeactivate(i)
        if rtcState(i) == 3:
            tkMessageBox.showwerror('Emergency',i[len(i)-1]+" がエラーを起こしました\nリセットします")
            rtcReset(i)
    for i in r_remotecomp:
        rtcDeactivate(i)


def rRTexit(event):
    #ボタン押し間違い処理
    try:
        Tree.get_node(['/', r_root]).reparse()
    except:
        try:
            Tree.add_name_server(r_root)
        except:
            tkMessageBox.showerror('Emergency',"ネームサーバを確認できません")
            return
    for i in r_localcomp:
        if  not rtcIsComponent(i):
            tkMessageBox.showerror('Emergency',"ユーザRTCが起動していません")
            return
    #ディアクティベートを促す
    for i in r_localcomp:
        if rtcState(i) != 1:
            tkMessageBox.showerror('Notification',"ユーザRTC動作中\nディアクティベートしてください")
            return
        else:
            rtcExit(i)
    time.sleep(1)
    for j in r_localexe:
        finishExe(j)
    confdestroy()

def configrationsetting():
    if root1 != None:
            tkMessageBox.showerror('Error',"すでにコンフィグレーション設定画面が\nあります")
            return        
    for i in commonexe:
        if not findProcess(i):
            tkMessageBox.showerror('Error',"ユーザRTCが起動していません")
            return
    root=""
    for i in r_localcomp:
        c=Tree.get_node(i)
        if c==None:
            root=s_root
            break
        else:
            root=r_root
    confResetting(root)            
    
def main():
    Bbasex = 10
    Bbasey = 35
    space = 35
    simX = 250
    if OS=="linux":
        lin_Ulab=-2
        lin_Clab=-1
        lin_button=-4
        lin_simb=-3
        lin_simlab=-2
        lin_Cmlab=5
        Llabelcolor = "VioletRed1"
        Rlabelcolor = "orange"
        Clabelcolor = "VioletRed1"
        simcho_labelcolor="DarkOliveGreen3"
    elif OS=="windows":
        lin_Ulab=0
        lin_Clab=0
        lin_button=0
        lin_simlab=0
        lin_simb=0
        lin_Cmlab=0
        Llabelcolor = "spring green"
        Rlabelcolor = "deep sky blue"
        Clabelcolor = "spring green"   
        simcho_labelcolor="medium purple"

    root = TK.Tk()
    root.title(u"ロボットプログラミングマネージャ")
    root.geometry("400x320+10+500")
    root.lift()
    root.attributes('-topmost',True)
    def closeButton():
        tkMessageBox.showerror('Error', "閉じるボタンは使わないでください")
    root.protocol('WM_DELETE_WINDOW', closeButton)
    def exitMain():
       root.destroy()
       Fexperi = open('anticipateYAML','w')
       Fexperi.write(Filename)
       Fexperi.close()
    def finishANDstart():
        for i in commonexe:
            if findProcess(i):
                tkMessageBox.showerror('Error', "ユーザRTCを終了してください")
                return
        try:
            os.remove('anticipateYAML')
        except:
            print "File nothing. No problem"
        subprocess.Popen(sys.argv[0], shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        root.destroy()
    #root.after_idle(root.attributes,'-topmost',False)

    #メニュー
    menu_root = TK.Menu(root)
    root.configure(menu = menu_root)

    menu_FILE = TK.Menu(menu_root,tearoff=False)
    menu_root.add_cascade(label='FILE',menu=menu_FILE,underline=0)
    menu_conf = TK.Menu(menu_root,tearoff=False)
    menu_root.add_cascade(label='Configuration',menu=menu_conf,underline=0)
    
    menu_FILE.add_command(label='Load YAML',under=0,command=finishANDstart)
    menu_FILE.add_command(label='EXIT',under=0,command=exitMain)
    
    menu_conf.add_command(label='Configure Set',under=0,command=configrationsetting)

    #左下ラベル
    global HelpMessage
    labely=215
    HelpMessage = TK.Label(root, text=u"ボタン上にカーソルを置くと\nそのボタンの処理を表示", foreground="#000000", bg=Llabelcolor, width=19+lin_Ulab, height=2, anchor=TK.N)
    HelpMessage.place(x=5, y=Bbasey + labely)
    #右下ラベル
    ip = socket.gethostbyname(socket.gethostname())
    ipex = TK.Label(root,text="あなたのIPアドレスは\n" + ip,bg=Clabelcolor,width=19+lin_Ulab,height=2,anchor=TK.N)
    ipex.place(x=255,y=Bbasey + labely)
    ###########################################
    #真ん中ラベル
    lposx = 150
    l1 = TK.Label(root,text="ロボット側起動",width=13+lin_Clab,bg=Rlabelcolor)
    l1.place(x=lposx,y=Bbasey+lin_Cmlab)
    l2 = TK.Label(root,text="起動＋ポート接続",width=13+lin_Clab,bg=Rlabelcolor)
    l2.place(x=lposx,y=Bbasey + 35+lin_Cmlab)
    l3 = TK.Label(root,text="開始",width=13+lin_Clab,bg=Rlabelcolor)
    l3.place(x=lposx,y=Bbasey + 70+lin_Cmlab)
    l4 = TK.Label(root,text="停止",width=13+lin_Clab,bg=Rlabelcolor)
    l4.place(x=lposx,y=Bbasey + 105+lin_Cmlab)
    l5 = TK.Label(root,text="終了＋ポート切断",width=13+lin_Clab,bg=Rlabelcolor)
    l5.place(x=lposx,y=Bbasey + 140+lin_Cmlab)
    l6 = TK.Label(root,text="ロボット側終了",width=13+lin_Clab,bg=Rlabelcolor)
    l6.place(x=lposx,y=Bbasey + 175+lin_Cmlab)

    def exitMain(event):
       root.destroy()
       Fexperi = open('anticipateYAML','w')
       Fexperi.write(Filename)
       Fexperi.close()
    ButtonC = TK.Button(text=u'マネージャ終了', width=13+lin_button, height=2)
    ButtonC.place(x=lposx, y=Bbasey + 210)
    ButtonC.bind("<Enter>", lambda x: Message("マネージャ終了"))
    ButtonC.bind("<ButtonRelease-1>", exitMain)
    ###########################################
    #実機側
    Real = TK.LabelFrame(text = u'実機',width = 140, height = 240, labelanchor = TK.NW)
    Real.place(x = 5, y = 10)

    ButtonR1 = TK.Button(text=u'ユーザRTC起動', width=17+lin_button,height=1)
    ButtonR1.place(x=Bbasex,y=Bbasey + space)
    ButtonR1.bind("<Enter>",lambda R0 : Message("ユーザRTC起動\nコンポーネント接続"))
    ButtonR1.bind("<ButtonRelease-1>", rStartController)


    ButtonR2 = TK.Button(text=u'アクティベート', width=17+lin_button,height=1)
    ButtonR2.place(x=Bbasex,y=Bbasey + space*2)
    ButtonR2.bind("<Enter>",lambda R0 : Message("実機アクティベート"))
    ButtonR2.bind("<ButtonRelease-1>", rActivate)

    ButtonR3 = TK.Button(text=u'ディアクティベート', width=17+lin_button,height=1)
    ButtonR3.place(x=Bbasex,y=Bbasey + space*3)
    ButtonR3.bind("<Enter>",lambda R0 : Message("実機ディアクティベート"))
    ButtonR3.bind("<ButtonRelease-1>",rDeactivate)

    ButtonR4 = TK.Button(text=u'ユーザRTC終了', width=17+lin_button,height=1)
    ButtonR4.place(x=Bbasex,y=Bbasey + space*4)
    ButtonR4.bind("<Enter>",lambda R0 : Message("ユーザRTC終了"))
    ButtonR4.bind("<ButtonRelease-1>",rRTexit)


    ########################################
    #シミュ側
    Sim = TK.LabelFrame(text = u'シミュレーション',width = 140, height = 240, labelanchor = TK.NW)
    Sim.place(x = 255, y = 10)

    comboS1 = ttk.Combobox(root, state='readonly',values=s_label, width=6,height=1)
    comboS1.place(x=Bbasex + 250,y=Bbasey+3)
    comboS1.current(0)

    ButtonS12 = TK.Button(text=u'シミュ', width=7+lin_simb,height=1)
    ButtonS12.place(x=Bbasex + simX + 70,y=Bbasey)
    ButtonS12.bind("<Enter>",lambda R0 : Message("コレオノイド\n支援RTCを起動"))
    ButtonS12.bind("<ButtonRelease-1>",lambda x: startSimulator(comboS1.get()))

    ButtonS2 = TK.Button(text=u'ユーザRTC起動', width=17+lin_button,height=1)
    ButtonS2.place(x=Bbasex + simX,y=Bbasey + space)
    ButtonS2.bind("<Enter>",lambda R1 : Message("ユーザRTC起動\nコンポーネント接続"))
    ButtonS2.bind("<ButtonRelease-1>",sStartController)

    S1 = TK.Label(root,text="コレオノイド側で\n操作してください",width=17+lin_simlab,height=4,bg=simcho_labelcolor)
    S1.place(x=Bbasex + 250,y=Bbasey + space*2)

    ButtonS3 = TK.Button(text=u'ユーザRTC終了', width=17+lin_button,height=1)
    ButtonS3.place(x=Bbasex + simX,y=Bbasey + space*4)
    ButtonS3.bind("<Enter>",lambda R0 : Message("ユーザRTC終了"))
    ButtonS3.bind("<ButtonRelease-1>",sRTexit)

    ButtonS4 = TK.Button(text=u'終了', width=17+lin_button,height=1)
    ButtonS4.place(x=Bbasex + simX,y=Bbasey + space*5)
    ButtonS4.bind("<Enter>",lambda R0 : Message("コレオノイド\n支援RTCを終了"))
    ButtonS4.bind("<ButtonRelease-1>",finishSimulator)

    root.mainloop()
    
    if OS=="windows":
        finishExe("cnoid-nameserver.exe")
        finishExe("choreonoid.exe")
    elif OS=="linux":
        finishExe("choreonoid")
    for i in s_remoteexe:
        finishExe(i)

##二つめのコンフィグレーション設定ウィンドウ
def confdestroy():
    global root1
    root1.destroy()
    root1=None  
def confResetting(nroot):
    combox = 10
    comboy = 25
    
    global root1
    root1 = TK.Toplevel()
    root1.title(u"コンフィグレーション設定")
    root1.geometry(str(400)+"x"+str(80)+"+410+500")
    menu_root1 = TK.Menu(root1)
    root1.configure(menu = menu_root1)
    menu_Option1 = TK.Menu(menu_root1,tearoff=False)
    menu_root1.add_cascade(label='Option',menu=menu_Option1,underline=0)
    menu_Option1.add_command(label='EXIT',under=0,command=confdestroy)

    combo = ttk.Combobox(root1, state='readonly',values=commonComp, width=40,height=1)
    combo.place(x=combox,y=comboy)
    combo.current(0)
            
    ButtonS12 = TK.Button(root1,text=u"表示", width=7,height=1)
    ButtonS12.place(x=combox+320,y=comboy-2)
    ButtonS12.bind("<ButtonRelease-1>",lambda x: confSET(combo.get(),nroot))

    root1.lift()
    root1.attributes('-topmost',True)

    root1.mainloop()
    


def confSET(comp,nroot):
    root1.geometry(str(400)+"x"+str(130)+"+410+500")
    Comps=[]
    if nroot == s_root:
        Comps = s_localcomp
    elif nroot == r_root:
        Comps = r_localcomp
    else:
        tkMessageBox.showerror('Emergency',"ネームサーバが起動していません")
        return        
    for i in Comps:
        if  not rtcIsComponent(i):
            tkMessageBox.showerror('Emergency',"ユーザRTCが起動していません")
            return
    confConfirm(comp,Comps,nroot)
        
def confConfirm(comp,Comps,nroot):
    confcomp=""
    radiox = 10
    radioy = 60
    for i in Comps:
        if str(i).find(comp) != -1:
            confcomp=i
    #print confcomp
    Tree.get_node(['/', nroot]).reparse()
    c = Tree.get_node(confcomp)
    #print c
    allconf = c.conf_sets.keys()
    #print allconf

    conf_dic={}
    for i in range(0,len(allconf),+1):
        conf_dic[i] = allconf[i]
    #print conf_dic
    global confnum
    confnum = TK.IntVar()
    confnum.set(None)
    
    global confButton
    if len(confButton) >= 1:
        for cb in confButton:
            cb.destroy()
        confButton=[]
    def confSet():
        #cを引数にすると上手くいかないのでコンポーネントをリストの状態で
        confSetMenu(confcomp,confnum.get(),conf_dic,nroot)    
    for key,val in conf_dic.iteritems():
        #TK.Radiobutton(root1,text = key, variable = confnum, value = val).place(x=radiox+x,y=radioy)
        rb=TK.Radiobutton(root1,text = val, variable = confnum, value = key,command=confSet)
        rb.pack(side="left",anchor=TK.NW,pady=60)
        confButton.append(rb)

     
    #前回のコンフィグレーション一覧を消す
    global confName
    global confVal
    if len(confName) or len(confVal) >= 1:
        for cn in confName:
            cn.destroy()
        for cv in confVal:
            cv.destroy()
        confName=[]
        confVal=[]
    global decideButton
    if decideButton != "":
        decideButton.destroy()
        decideButton=""

#conf一覧        
def confSetMenu(comp,dkey,conf_dic,nroot):
    #ボタン削除
    global decideButton
    if decideButton != "":
        decideButton.destroy()
        decideButton=""
 
    Tree.get_node(['/', nroot]).reparse()
    Comp = Tree.get_node(comp)

    aconf = conf_dic[dkey]

    #二回すればうまくいく
    Comp.reparse_conf_sets()
    Comp.activate_conf_set(aconf)
    Comp.reparse_conf_sets()
    Comp.activate_conf_set(aconf)
    #print aconf,Comp.active_conf_set_name
    
    activeconf = Comp.active_conf_set.data
    #print activeconf
    
    h=0
    global confName
    global confVal

    for key,val in activeconf.iteritems():
        l=TK.Label(root1,text=key)
        l.place(x=5,y=90+h)
        confName.append(l)
        e=TK.Entry(root1,width=10)
        e.insert(TK.END,str(val))
        e.place(x=300,y=90+h)
        confVal.append(e)
        h+=20

    cnum = len(activeconf)
    root1y = 130
    for i in range(cnum):
        root1y += 23
    root1.geometry(str(400)+"x"+str(root1y)+"+410+500")
    
    decideButton = TK.Button(root1,text='決定', width=6,height=1)
    decideButton.place(x=400-55,y=root1y-35)
    decideButton.bind("<ButtonRelease-1>",lambda x: decideConf(confnum.get(),Comp,conf_dic,nroot))

def decideConf(numconf,Comp,conf_dic,nroot):
    
    Tree.get_node(['/', nroot]).reparse()
    nowconf = conf_dic[numconf]

    confs={}
    for uconfName,uconfVal in zip(confName,confVal):
        confs[uconfName["text"]] = uconfVal.get()
    tval=[]

    #print Comp.active_conf_set.data
    #print Comp.active_conf_set_name
    Comp.reparse_conf_sets()
    for param,val in confs.iteritems():
        #print nowconf,param,val
        Comp.set_conf_set_value(nowconf,param,val)
        #print Comp.active_conf_set.data
        tval.append(val)

    Comp.activate_conf_set(nowconf)
    for val,uval in zip(confVal,tval):
        val.delete(0,TK.END)
        val.insert(TK.END,str(uval))
    #print Comp.active_conf_set.data


def filecheck(filename):
    try:
        global data
        stream = open(filename,"r")
        data = yaml.load(stream)
    except:
        tkMessageBox.showerror('Emergency',filename+u"の入力を確認できません")
        sys.exit()
    check_common={"userComps":["command","rtc"]}
    check_real={"server":[],"userCompConf":[],"remoteComps":["rtc"],"connections":[]}
    check_sim={"server":[],"userCompConf":[],"supportComps":["command","rtc"],"choreonoidCommand":[],"projects":["path","label"],"choreonoidComps":["rtc"],"connections":[]}
    fail=""
    #commonミスのチェック
    try:
        for key in check_common.keys():
            fail=key
            a=data["common"][key]
            for j in a:
                for value in check_common.values():
                    for i in value:
                        fail=i
                        confirm = j[i]
    except:
        tkMessageBox.showerror('Emergency',"："+fail+"の記述ミスです")
        sys.exit()
    #realミスのチェック
    try:
        for key in check_real.keys():
            fail=key
            first=data["real"][key]
            if key=="remoteComps":
                for rtc in check_real[key]:
                    fail=rtc
                    for sec in first:
                        confirm=sec[rtc]
    except:
        tkMessageBox.showerror('Emergency',"real"+"："+fail+"の記述ミスです")
        sys.exit()
    #simミスのチェック
    try:
        for key in check_sim.keys():
            fail=key
            first=data["sim"][key]
            if key=="supportComps" or key=="projects" or key=="choreonoidComps":
                for rtc in check_sim[key]:
                    fail=rtc
                    for sec in first:
                        confirm=sec[rtc]
    except:
        tkMessageBox.showerror('Emergency',"sim"+"："+fail+"の記述ミスです")
        sys.exit()

#読み込み
def compInsert(comp,root):
    returncomp=[]
    for i in comp:
        compnode=[]
        compnode.append("/")
        compnode.append(root)
        if i.find("${HOST_CXT}/")!=-1:
            i=i.replace("${HOST_CXT}/","")
            compnode.append(socket.gethostname() + ".host_cxt")
        compnode.append(i)
        returncomp.append(compnode)
    return returncomp

def yamlread():

    global commonComp
    global commonexe

    global pyComp
     
    global r_root
    global r_localexe
    global r_localcomp
    global r_remotecomp
    global r_rtcon

    global s_root
    global s_simexe
    global s_label
    global simexe
    global s_remoteexe
    global simComp
    global s_localexe
    global s_localcomp
    global s_remotecomp
    global s_rtcon

    #common
    #commonexe=[]
    
    for i in data["common"]["userComps"]:
        commonexe.append(i["command"])
        if i["command"].find(".py") != -1:
            pyComp.append(i["rtc"])
        commonComp.append(i["rtc"])

    #real
    real =data["real"]
    r_root=real["server"]
    r_userCompConf=real["userCompConf"]
    for i in commonexe:
        r_localexe.append(i + " -f "+r_userCompConf)
    for i in commonComp:
        r_localcomp.append("${HOST_CXT}/"+i)
    for i in real["remoteComps"]:
        r_remotecomp.append(i["rtc"])
        r_rtcon = real["connections"]
    
    #sim
    sim = data["sim"]
    s_root=sim["server"]
    s_userCompConf = sim["userCompConf"]
    for i in sim["supportComps"]:
        simexe.append(i["command"])
        if i["command"].find(".py") != -1:
            pyComp.append(i["rtc"])
        simComp.append(i["rtc"])
    for i in commonexe:
        s_localexe.append(i+ " -f "+ s_userCompConf)
    s_remoteexe = simexe
    s_choreonoid = sim["choreonoidCommand"]
    for i in sim["projects"]:
        s_label.append(i["label"])
        s_simexe[i["label"]]=s_choreonoid+" "+i["path"]
    s_localcomp = commonComp
    for i in sim["choreonoidComps"]:
        s_remotecomp.append(i["rtc"])
        s_remotecomp.extend(simComp)
    s_rtcon = sim["connections"]

    #コンポーネントをリスト化
    r_localcomp = compInsert(r_localcomp,r_root)
    r_remotecomp = compInsert(r_remotecomp,r_root)
    s_localcomp = compInsert(s_localcomp,s_root)
    s_remotecomp = compInsert(s_remotecomp,s_root)
    pyComp = compInsert(pyComp,s_root)

    
def fileread():
    Filesel = TK.Tk()
    Filesel.withdraw()

    fName = [('設定ファイル','*.yaml')]
    Dir = './'

    try:
        Filename = tkFileDialog.askopenfilename(filetypes=fName,initialdir=Dir)
    except:
        Filesel.destroy()
        sys.exit()
    print Filename
    if Filename == "":
        sys.exit()
    Filesel.destroy()
    return Filename

if __name__ == "__main__":
  args = sys.argv
  if len(args) >= 2:
      Filename = args[1]
  else:
      try:
          lastfile = open('anticipateYAML','r')
          Filename = lastfile.readline()
          lastfile.close()
      except:
          Filename = fileread()

  filecheck(Filename)
  yamlread()
  main()
