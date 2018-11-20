import pygame
import scapy.all as scp
import scapy

INTF = 'Qualcomm Atheros AR956x Wireless Network Adapter'
MHRCC_MAC = 'b1:ea:db:1e:ad:'

# Define some colors
BLACK    = (   0,   0,   0)
WHITE    = ( 255, 255, 255)
target_car = 'a'
next_hop_car = 'a'
# for control car
def send_to_car(src_mac, next_hop='61'):
    packet = scapy.layers.l2.Ether(dst=MHRCC_MAC + next_hop, src=src_mac)/scapy.all.Raw('blead')
    scp.sendp(packet, iface=INTF)

def to_hex(char):
    return '{:x}'.format(ord(char))

def send_control(direction, target, next_hop='61'):
    src_mac = '{}:{}:5a:db:5a:db'.format(to_hex(direction), to_hex(target))
    print(src_mac)
    send_to_car(src_mac, next_hop=next_hop)
# This is a simple class that will help us print to the screen
# It has nothing to do with the joysticks, just outputting the
# information.

class TextPrint:
    def __init__(self):
        self.reset()
        self.font = pygame.font.Font(None, 20)

    def print(self, screen, textString):
        textBitmap = self.font.render(textString, True, BLACK)
        screen.blit(textBitmap, [self.x, self.y])
        self.y += self.line_height
        
    def reset(self):
        self.x = 10
        self.y = 10
        self.line_height = 15
        
    def indent(self):
        self.x += 10
        
    def unindent(self):
        self.x -= 10
    

pygame.init()
 
# Set the width and height of the screen [width,height]
size = [500, 700]
screen = pygame.display.set_mode(size)

pygame.display.set_caption("My Game")

#Loop until the user clicks the close button.
done = False

# Used to manage how fast the screen updates
clock = pygame.time.Clock()

# Initialize the joysticks
pygame.joystick.init()
    
# Get ready to print
textPrint = TextPrint()

# -------- Main Program Loop -----------
while done==False:
    # EVENT PROCESSING STEP
    for event in pygame.event.get(): # User did something
        if event.type == pygame.QUIT: # If user clicked close
            done=True # Flag that we are done so we exit this loop
        
        # Possible joystick actions: JOYAXISMOTION JOYBALLMOTION JOYBUTTONDOWN JOYBUTTONUP JOYHATMOTION
        # if event.type == pygame.JOYBUTTONDOWN:
        #     print("Joystick button pressed.")
        # if event.type == pygame.JOYBUTTONUP:
        #     print("Joystick button released.")
            
 
    # DRAWING STEP
    # First, clear the screen to white. Don't put other drawing commands
    # above this, or they will be erased with this command.
    screen.fill(WHITE)
    textPrint.reset()

    # Get count of joysticks
    joystick_count = pygame.joystick.get_count()

    textPrint.print(screen, "Number of joysticks: {}".format(joystick_count) )
    textPrint.indent()
    
    # For each joystick:
    for i in range(joystick_count):
        joystick = pygame.joystick.Joystick(i)
        joystick.init()
    
        textPrint.print(screen, "Joystick {}".format(i) )
        textPrint.indent()
    
        # Get the name from the OS for the controller/joystick
        name = joystick.get_name()
        textPrint.print(screen, "Joystick name: {}".format(name) )
        
        # Usually axis run in pairs, up/down for one, and left/right for
        # the other.
        axes = joystick.get_numaxes()
        textPrint.print(screen, "Number of axes: {}".format(axes) )
        textPrint.indent()
        
        for i in range( axes ):
            axis = joystick.get_axis( i )
            textPrint.print(screen, "Axis {} value: {:>6.3f}".format(i, axis) )
        textPrint.unindent()
            
        buttons = joystick.get_numbuttons()
        textPrint.print(screen, "Number of buttons: {}".format(buttons) )
        textPrint.indent()

        for i in range( buttons ):
            button = joystick.get_button( i )
            textPrint.print(screen, "Button {:>2} value: {}".format(i,button) )
        textPrint.unindent()
            
        # Hat switch. All or nothing for direction, not like joysticks.
        # Value comes back in an array.
        hats = joystick.get_numhats()
        textPrint.print(screen, "Number of hats: {}".format(hats) )
        textPrint.indent()

        for i in range( hats ):
            hat = joystick.get_hat( i )
            textPrint.print(screen, "Hat {} value: {}".format(i, str(hat)) )
        textPrint.unindent()
        
        textPrint.unindent()

        textPrint.print(screen, "target car {} next hop {}".format(target_car,next_hop_car) )

        if(pygame.joystick.Joystick(0).get_button(0)):
            print("LEFT")
            send_control('a',target_car,next_hop=next_hop_car)
        if(pygame.joystick.Joystick(0).get_button(1)):
            print("BACKWARD")
            send_control('s',target_car,next_hop=next_hop_car)
        if(pygame.joystick.Joystick(0).get_button(2)):
            print("RIGHT")
            send_control('d',target_car,next_hop=next_hop_car)
        if(pygame.joystick.Joystick(0).get_button(3)):
            print("FORWARD")
            send_control('w',target_car,next_hop=next_hop_car)
        if(pygame.joystick.Joystick(0).get_button(4)):
            print("LB")
            target_car='a'
        if(pygame.joystick.Joystick(0).get_button(5)):
            print("RB")
            target_car='b'
        if(pygame.joystick.Joystick(0).get_button(6)):
            print("LT")
            next_hop_car='a'
        if(pygame.joystick.Joystick(0).get_button(7)):
            print("RT")
            next_hop_car='b'
        if(pygame.joystick.Joystick(0).get_button(9)):
            print("EMER")
            send_control('e',target_car,next_hop=next_hop_car)
            

    
    # ALL CODE TO DRAW SHOULD GO ABOVE THIS COMMENT
    
    # Go ahead and update the screen with what we've drawn.
    pygame.display.flip()

    # Limit to 20 frames per second
    clock.tick(20)
    
# Close the window and quit.
# If you forget this line, the program will 'hang'
# on exit if running from IDLE.
pygame.quit ()
