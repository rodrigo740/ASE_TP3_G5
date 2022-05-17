import PySimpleGUI as sg

NAME_SIZE = 23


def name(name):
    dots = NAME_SIZE-len(name)-2
    return sg.Text(name, justification='r', pad=(0, 0), font='Courier 10')


layout = [[sg.Text('My Window')],
          [sg.Input(key='-IN-'), sg.Button('MyBrowse', key='-BROWSE-')],
          [name('Temperature'), sg.Frame(
              layout=[[sg.Text('temp here')]], title='Temp:')],
          [name('On/Off'), sg.Button('Button')],
          [name('Duty Cycle'), sg.Slider((0, 100), orientation='h', s=(10, 15))],
          [sg.Button('Go'), sg.Button('Exit')],
          ]

# Create the window
window = sg.Window("Raspberry PI 4", layout, margins=(150, 150))

# Create an event loop
while True:
    event, values = window.read()
    print(event, values)
    if event == sg.WIN_CLOSED or event == 'Exit':
        break
    if event == '-BROWSE-':
        file = sg.popup_get_file('', no_window=True)
        window['-IN-'].update(file)

window.close()
