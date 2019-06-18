#pragma once   //maybe should be static class
#include "display.h"
#include "game.h"


	void mouse_callback(GLFWwindow* window,int button, int action, int mods)
	{	
		if(action == GLFW_PRESS )
		{
			Game *scn = (Game*)glfwGetWindowUserPointer(window);
			double x2,y2;
			glfwGetCursorPos(window,&x2,&y2);
			scn->picking((int)x2,(int)y2);
		}
	}
	
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);
		scn->shapeTransformation(scn->zCameraTranslate,(float) yoffset);		
	}
	
	vec3 z(0, 0, 1);
	vec3 x(1, 0, 0);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);

		if(action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			switch (key)
			{
				case GLFW_KEY_ESCAPE:			
					glfwSetWindowShouldClose(window,GLFW_TRUE);
				break;
				case GLFW_KEY_P:
					scn->changeMode();
				break;
				case GLFW_KEY_O:
					scn->HideShape(0);
					if(scn->IsActive())
						scn->Deactivate();
					else
						scn->Activate();
					break;
				case GLFW_KEY_A:
					scn->change_movement(-0.001f);
					break;
				case GLFW_KEY_S:
					scn->change_movement(0.001f);
					break;
				case GLFW_KEY_D:
					scn->SetDirection();
					break;
				case GLFW_KEY_C:
					scn->cameramode = !scn->cameramode;
					break;
				case GLFW_KEY_RIGHT: 
					if (scn->cameramode)
						scn->shapeTransformation(scn->zLocalRotate, 5.0f);
					else
						scn->OpositeDirectionRot(z, 10.f);
					break;
				case GLFW_KEY_LEFT:
					if (scn->cameramode)
						scn->shapeTransformation(scn->zLocalRotate,-5.0f);
					else
						scn->OpositeDirectionRot(z, 10.f);
					break;
				case GLFW_KEY_UP:
					if (scn->cameramode)
						scn->shapeTransformation(scn->xLocalRotate,5.0f);
					else
						scn->OpositeDirectionRot(x, 5.f);
					break;
				case GLFW_KEY_DOWN:
					if (scn->cameramode)
						scn->shapeTransformation(scn->xLocalRotate,-5.0f);
					else
						scn->OpositeDirectionRot(x, 5.f);
					break;

			default:
				break;
			}
		}
	}
	

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);

		scn->updatePosition((float)xpos,(float)ypos);
		if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			scn->mouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
		}
		else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			scn->mouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
		}

	}

	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);

		scn->resize(width,height);
		
	}

	void init(Display &display)
	{
		display.addKeyCallBack(key_callback);
		display.addMouseCallBacks(mouse_callback,scroll_callback,cursor_position_callback);
		display.addResizeCallBack(window_size_callback);

	}
