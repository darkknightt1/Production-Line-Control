from tkinter import *
import threading
import socket
def Socket_Setup():
    global host
    global port
    global s
    global address

    host = '192.168.1.15'  # the Arduino
    port = 5005
    local_port = 5005
    address = (host, port)
    s  = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind((('0.0.0.0', local_port)))
    s.connect(address)  #receiving from arduino
   # s2.connect(address) #sending to arduino
   # s2.sendto(str.encode('S'), address) #to start
   # data = s2.recv(1024)

    #while ( len( data ) < 0):
     #   s2.sendto(str.encode('S'), address)
       # data = s.recv(1024)
def Socket_loop():
    global DATA_RECEIVED, s, NonMetal_counter, Metal_counter ,State
    DATA_RECEIVED = False
    NonMetal_counter = 0
    Metal_counter =0
    State =0
    while True:
        #print("thread1")
        data = s.recv(1024)
        if len(data) > 0:
            DATA_RECEIVED = True
            #print (data.decode("utf-8"))
            if data.decode("utf-8") == 'M':
                Metal_counter += 1
                #print(Metal_counter)
            elif data.decode("utf-8") == 'N':
                NonMetal_counter += 1
                #print(NonMetal_counter)
            elif data.decode("utf-8") == 'F':
                State = 1
            elif data.decode("utf-8") == 'S':
                State = 2
            elif data.decode("utf-8") == 'O':
                State = 3
            elif data.decode("utf-8") == 'P':
                State = 4

        else:
            DATA_RECEIVED = False
def update_gui():
    global DATA_RECEIVED, Metal_counter, NonMetal_counter, State
    if DATA_RECEIVED:
        if Metal_counter == 1:
            update_label_color(Metallbl1)
        elif Metal_counter == 2:
            update_label_color(Metallbl2)
        elif Metal_counter == 3:
            update_label_color(Metallbl3)
        elif Metal_counter == 4:
            update_label_color(Metallbl4)

        if NonMetal_counter == 1:
            update_label_color(NonMetallbl1)
        elif NonMetal_counter == 2:
            update_label_color(NonMetallbl2)
        elif NonMetal_counter == 3:
            update_label_color(NonMetallbl3)
        elif NonMetal_counter == 4:
            update_label_color(NonMetallbl4)

        if State == 2:
            StateLbl["fg"] = "white"
            StateLbl["text"] = "Sorting"
        elif State == 1:
            StateLbl["fg"] = "white"
            StateLbl["text"] = "Feeding"
        elif State == 3:
            StateLbl["fg"] = "white"
            StateLbl["text"] = "Homing"
        elif State == 4:
            StateLbl["fg"] = "white"
            StateLbl["text"] = "Packaging"


        DATA_RECEIVED = False

    root.after(100, update_gui)  # Schedule the function to be called after 100 milliseconds
def update_label_color(label):
    if label["bg"] == "#003839":
        label["bg"] = "Green"
def Send_Start_BTN():
    if StartBTN["text"]== " START":
        StartBTN["text"]= " STOP"
        StartBTN["fg"] = "white"
        StartBTN["bg"] = "red"
        #s.sendto(str.encode('S'), address)

    elif StartBTN["text"]== " STOP":
        StartBTN["text"]= " START"
        #StartBTN["fg"] = "white"
        StartBTN["bg"] = "#00863F"
        #s.sendto(str.encode('F'), address)
def GUI_Loop():
    global root ,Metallbl1,Metallbl2,Metallbl3,Metallbl4,NonMetallbl1,NonMetallbl2,NonMetallbl3,NonMetallbl4,StateLbl,StartBTN
    root = Tk()
    root.geometry("1024x1024")
    root.configure(bg="#00a1a3")
    root.title("Production Line")

    IMG_bg = PhotoImage(file="WhatsApp Image 2024-01-24 at 00.28.47_5380af78.png")
    IMG_label = Label(root, width="1000", height="800", image=IMG_bg)
    IMG_label.place(x=0, y=0)


    StateLblHDR = Label(root, text="STATE:", font=('Helvetica', 18, 'bold'), width="15", height="1", fg="black", bg="#00a1a3")
    StateLbl = Label(root, text="Connected.", font=('calibri', 38), width="15", height="1", fg="white", bg="#00a1a3")

    StartBTN= Button(root, text= " START" ,font=('Helvetica', 18, 'bold'), width="15", height="1", fg="white", bg="#00863F", command=Send_Start_BTN)
    MetalNameLbl = Label(root, text="METAL", font=('arial', 12, 'bold'), width="15", height="2", fg="white", bg="#00a1a3")
    Metallbl1 = Label(root, width="10", height="5", bg="#003839")
    Metallbl2 = Label(root, width="10", height="5", bg="#003839")
    Metallbl3 = Label(root, width="10", height="5", bg="#003839")
    Metallbl4 = Label(root, width="10", height="5", bg="#003839")

    NonMetalNameLbl = Label(root, text="NON-METAL", font=('arial', 12, 'bold'), width="15", height="2", fg="white", bg="#00a1a3")
    NonMetallbl1 = Label(root, width="10", height="5", bg="#003839")
    NonMetallbl2 = Label(root, width="10", height="5", bg="#003839")
    NonMetallbl3 = Label(root, width="10", height="5", bg="#003839")
    NonMetallbl4 = Label(root, width="10", height="5", bg="#003839")

    MetalNameLbl.place(x=1300, y=560)
    Metallbl1.place(x=1300, y=380)
    Metallbl4.place(x=1300, y=480)
    Metallbl2.place(x=1400, y=380)
    Metallbl3.place(x=1400, y=480)




    NonMetalNameLbl.place(x=1060, y=560)
    NonMetallbl1.place(x=1050, y=380)
    NonMetallbl4.place(x=1050, y=480)
    NonMetallbl2.place(x=1150, y=380)
    NonMetallbl3.place(x=1150, y=480)

    StateLblHDR.place(x=1060, y=175)
    StateLbl.place(x=1080, y=210)

    StartBTN.place(x=1160 , y = 700)

    Socket_thread = threading.Thread(target=Socket_loop, daemon=True)
    Socket_thread.start()

    update_gui()  # Start the update_gui function
    root.mainloop()

#main application
Socket_Setup()
GUI_Loop()

