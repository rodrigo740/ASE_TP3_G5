from random import randint
import PySimpleGUI as sg

def make_window(theme):
    sg.theme(theme)
    menu_def = [['&Application', ['E&xit']],
                ['&Help', ['&About']] ]
    right_click_menu_def = [[], ['Edit Me', 'Versions', 'Nothing','More Nothing','Exit']]
  
    #addr_written
    wrt_addrs = [ x for x in range(0, 512, 8) ]
    wrt_addrs = tuple(wrt_addrs)

    input_layout =  [ 
                [sg.Text(text='PWM_1 : ',size=(12, 1)), 
                sg.Text(size=(5,2), text='duty cycle'), 
                sg.Slider(range=(0,100), size=(13,7), orientation='h', enable_events=True, default_value=50, key='-pwm1-'),],
                
                [sg.Text('PWM_2 : ', size=(12, 1)), 
                sg.Text(size=(5,2), text='duty cycle'), 
                sg.Slider(range=(0,100), size=(13,7), orientation='h', enable_events=True, default_value=50, key='-pwm2-'),],
                
                [sg.Text(text='Temperature : ',size=(12, 1)),
                sg.Text(size=(5,2), text='output'),  
                sg.Input(s=(15,1), default_text="0", readonly=True, key='-Temp-'),
                sg.Image(data=sg.DEFAULT_BASE64_LOADING_GIF, enable_events=True, key='-GIF-IMAGE-')],

                [sg.Text(text='SPI : ',size=(12, 1))], 
                [sg.Text(text='value',size=(5, 2)),
                sg.Input(s=(15,1), key='-Wrtval-'),
                sg.Text(text='address',size=(7, 2)),
                sg.Input(s=(15,1), key='-Wrtaddr-'),
                sg.Button('Write', enable_events=True)],

                [sg.Text(text='address', size=(7, 2)),
                sg.Combo(values= wrt_addrs,size=(7,1), readonly=True, k='-ComboAddrs-'),
                sg.Button('Read', enable_events=True),
                sg.Text(text='value',size=(5, 2)),
                sg.Input(s=(15,1), default_text="0", readonly=True, key='-Readval-')],
                
                [sg.Button('Start', button_color=('white', 'springgreen4'), key='-Start-'),
                sg.Button('Stop', button_color=('white', 'firebrick3'), key='-Stop-')]
                ]



    theme_layout = [[sg.Text("Choose a different theme")],
                    [sg.Listbox(values = sg.theme_list(), size =(20, 12), key ='-THEME LISTBOX-', enable_events = True)],
                    [sg.Button("Set Theme")]]


    layout = [ [sg.MenubarCustom(menu_def, key='-MENU-', font='Courier 15', tearoff=True)],
                [sg.Text('Assignment 2 - Raspberry Pi 4B', size=(38, 1), justification='center', font=("Helvetica", 16), relief=sg.RELIEF_RIDGE, k='-TEXT HEADING-', enable_events=True)]]
    layout +=[[sg.TabGroup([[  sg.Tab('Settings', input_layout),
                                sg.Tab('Theming', theme_layout)]], key='-TAB GROUP-', expand_x=True, expand_y=True),

               ]]
    layout[-1].append(sg.Sizegrip())
    window = sg.Window('Pi Project Interface', layout, size=(600,500), right_click_menu=right_click_menu_def, right_click_menu_tearoff=True, grab_anywhere=True, resizable=True, margins=(0,0), use_custom_titlebar=True, finalize=True, keep_on_top=True,)
    window.set_min_size(window.size)
    return window

def main():

    
    window = make_window(sg.theme())
    start = False
    # This is an Event Loop 
    while True:
        event, values = window.read(timeout=200)
        window['-GIF-IMAGE-'].update_animation(sg.DEFAULT_BASE64_LOADING_GIF, time_between_frames=100)
        if event not in (sg.TIMEOUT_EVENT, sg.WIN_CLOSED):
            print('============ Event = ', event, ' ==============')
            print('-------- Values Dictionary (key=value) --------')
            for key in values:
                print(key, ' = ',values[key])
        if event in (None, 'Exit'):
            print("[LOG] Clicked Exit!")
            break
        elif event == 'About':
            print("[LOG] Clicked About!")
            sg.popup('RaspeberryPy 4B Model project',
                     'Present interface to control four ',
                     '2x PWM the duty cycle of the led',
                     'Temperature sensor shows the output value read',
                     'SPI to write and read the EPPROM',
                     'Project made by Marco Ramos 93388 and Rodrigo Martins 93264', keep_on_top=True)
        elif event == "Set Theme":
            print("[LOG] Clicked Set Theme!")
            theme_chosen = values['-THEME LISTBOX-'][0]
            print("[LOG] User Chose Theme: " + str(theme_chosen))
            window.close()
            window = make_window(theme_chosen)
        elif event == "pwm1":
            print("pwm1 value = {v}".format(v=values['-pwm1-']))
        elif event == "pwm2":
            print("pwm2 value = {v}".format(v=values['-pwm2-']))
        elif event == "Write":
            print('spi: write {v} in addr {a}'.format(v=values['-Wrtval-'], a=values['-Wrtaddr-']) )
        elif event == "Read":
            print('Read spi from address = {a}'.format(a=values['-ComboAddrs-']))
            window['-Readval-'].update(123)
        elif event == "-Start-":
            start =  True
        elif event == "-Stop-":
            start =  False

        if start:
            window['-Temp-'].update(randint(18, 33))


    window.close()
    exit(0)

if __name__ == '__main__':
    sg.theme('BrownBlue')
    main()