import glfw
from OpenGL.GL import *
import numpy as np

gComposedM = np.array([[1.,0.,0.],
                      [0.,1.,0.],
                      [0.,0.,1.]])
newM = np.array([[1.,0.,0.],
                 [0.,1.,0.],
                 [0.,0.,1.]])

def render(T):
    glClear(GL_COLOR_BUFFER_BIT)
    glLoadIdentity()
    # draw cooridnate
    glBegin(GL_LINES)
    glColor3ub(255, 0, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([1.,0.]))
    glColor3ub(0, 255, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([0.,1.]))
    glEnd()
    # draw triangle
    glBegin(GL_TRIANGLES)
    glColor3ub(255, 255, 255)
    glVertex2fv((T @ np.array([.0,.5,1.]))[:-1])
    glVertex2fv((T @ np.array([.0,.0,1.]))[:-1])
    glVertex2fv((T @ np.array([.5,.0,1.]))[:-1])
    glEnd()

def key_callback(window, key, scancode, action, mods):
    global gComposedM
    global newM

    if action is not glfw.PRESS:
        return

    if key == glfw.KEY_Q:
        newM = np.array([[1.,0.,-0.1],
                         [0.,1.,0.],
                         [0.,0.,1.]])
        gComposedM = newM @ gComposedM
    elif key == glfw.KEY_E :
        newM = np.array([[1.,0.,0.1],
                         [0.,1.,0.],
                         [0.,0.,1.]])
        gComposedM = newM @ gComposedM
    elif key == glfw.KEY_A:
        newM = np.array([[np.cos(10 * np.pi/180), -np.sin(10 * np.pi/180),0.],
                         [np.sin(10 * np.pi/180), np.cos(10 * np.pi/180),0.],
                         [0.,         0.,        1.]])
        gComposedM = gComposedM @ newM
    elif key == glfw.KEY_D:
        newM = np.array([[np.cos(-10 * np.pi/180), -np.sin(-10 * np.pi/180),0.],
                         [np.sin(-10 * np.pi/180), np.cos(-10 * np.pi/180),0.],
                         [0.,         0.,        1.]])
        gComposedM = gComposedM @ newM
    elif key == glfw.KEY_1:
        newM = np.identity(3)
        gComposedM = np.identity(3)
    else :
        newM = np.identity(3)
        

def main():
    global gComposedM
    global newM

    if not glfw.init():
        return
    window = glfw.create_window(480,480,"2014005032-4-1", None,None)
    if not window:
        glfw.terminate()
        return
    glfw.set_key_callback(window, key_callback)
    glfw.make_context_current(window)

    while not glfw.window_should_close(window):
        glfw.poll_events()

        render(gComposedM)

        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()
