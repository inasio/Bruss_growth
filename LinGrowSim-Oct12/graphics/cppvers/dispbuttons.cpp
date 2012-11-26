/*
===============================================================================

	FILE:  dispbuttons.cpp
	
	PROJECT:
	
		GLOW widgets for disp_buttons
	
	CONTENTS:
	
		Controls window
	
	PROGRAMMERS:
	
		DH, based on Daniel Azuma <dazuma@kagi.com>
	
	COPYRIGHT:
	
		Copyright (C) 2000  Daniel Azuma  (dazuma@kagi.com)
		
		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.
		
		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
		GNU Lesser General Public License for more details.
		
		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to
			Free Software Foundation, Inc.
			59 Temple Place, Suite 330
			Boston, MA 02111-1307 USA
	
	VERSION:
	
		The GLOW Toolkit tutorial -- version 1.0.3  (10 September 2000)
	
	CHANGES:
	
		Original release (DA)

===============================================================================
*/


/*
===============================================================================
	Headers
===============================================================================
*/
#include <cstdio>
#include <algorithm>

using namespace std;

#include "glowViewTransform.h"

GLOW_NAMESPACE_USING

#include "dispbuttons.h"
#include "types.h"
#include "display.h"
#include "globals.h"
#include "mandelgizmo.h"
#include "glow.h"
#include <GL/glut.h>
/*
===============================================================================
	External things defined in dispbuttons.h
	that we need access to
===============================================================================
*/

unsigned char* _image1;
unsigned char* _image2;
int width, height;
//int picturecounter;
//int gizmowindname;
 
GlowPushButtonWidget* _printButton;
GlowPushButtonWidget* _previousButton;
GlowPushButtonWidget* _nextButton;
GlowPushButtonWidget* _goButton;
GlowPushButtonWidget* _resetButton;
GlowPushButtonWidget* _scaledownButton;
GlowPushButtonWidget* _scaleupButton;
GlowPushButtonWidget* _gizmoButton;
GlowCheckBoxWidget* _gridCheckBox;
GlowSliderWidget* _scaleslider;
GlowLabeledTextFieldWidget* _jumptoframe;

MandelGizmoWindow*   _firstgizmowind;
MandelGizmoSubwindow* _gizmoPanel;

GlowQuickPaletteWindow* _controlWindow;
//GlowQuickPaletteWindow* _conWin2;
Glow::KeyCode key;

int  gizmocounter=0;
int jumpnum;

char jumpmsg[100], newmsg[100];



/*
===============================================================================
	Methods of disp_buttons
===============================================================================
*/

//DispButtons::OnWidgetKeyboard(Glow::KeyCode key, int x, int y, Glow::Modifiers modifiers)
//{
  // need some guts here - define key as enterKey?   
//}

/*static void Key( unsigned char key, int x, int y )
{
  (void) x;
  (void) y;

  switch (key) {
        case 'LF':	
             sprintf(newmsg, "newmsg from field %s", _jumptoframe->GlowTextFieldWidget::GetTextChars());
              
    cout << "cout newmsg:" << newmsg << ", that was it\n";
	   break; 
  }
}*/

// Constructor

DispButtons::DispButtons(
	int mainWindowID)
{
	GLOW_DEBUGSCOPE("DispButtons::DispButtons");
	
	_mainWindowID = mainWindowID;
        new MandelGizmoWindow(_image1, _image2, width, height);
	
	//Glow::SetIdleFunc(GlowSubwindow::Refresh); 
     
	// Create a control panel window
	_controlWindow = new GlowQuickPaletteWindow("Controls");
	
        _controlWindow->GlowSubwindow::SetEventMask(Glow::mouseEvents | Glow::keyboardEvents | Glow::dragEvents);


	// Add controls
	// First, we'll put a little blurb at the top of the window
	_controlWindow->AddLabel(
		"Button Control of Displayer");
	
	// The rest of the window appears in panels. The panels are arranged
	// vertically, but widgets are arranged horizontally within each panel
	    GlowQuickPanelWidget* vpanel = _controlWindow->AddArrangingPanel(
		GlowQuickPanelWidget::vertical);
	
	// First we have a few general controls

           GlowQuickPanelWidget* panel = vpanel->AddPanel(
	        GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::horizontal);


	//	GlowQuickPalette* panel = _controlWindow->AddPanel(
	//    GlowQuickPanelWidget::etchedStyle, "Main");




	// Quit button - could also use AddDismissPushButton?

	_quitButton = panel->AddPushButton("Quit", this);

	// A separator
	   panel->AddSeparator();
	
	// _controlWindow->Pack();

	// Print button
	// Separate panel for print button, commented out.

	//		panel = hpanel->AddPanel(
	//GlowQuickPanelWidget::loweredStyle, "",
	//GlowQuickPanelWidget::vertical);
    
                _printButton = panel->AddPushButton("Print", this);


	//  new panel for show grid checkbox  
		panel = vpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::horizontal);

		// show grid checkbox 

	  _gridCheckBox =  panel->AddCheckBox(
                  "Show Grid", GlowCheckBoxWidget::off, this);


	  // new panel for next/previous buttons

		panel = vpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::horizontal);
	
	   // next/previous buttons     
	
           _previousButton = panel->AddPushButton("Previous",this);

	// A separator
	   panel->AddSeparator();

           _nextButton = panel->AddPushButton("Next",this);

	   // Input window, for jump to numbered frame 

	        panel = vpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::horizontal);

		char* initmsg;
          
		sprintf(initmsg,"%d", picturecounter);

    _jumptoframe = panel->GlowQuickPalette::AddTextField(50, initmsg, "Go to Frame"); 
		
   panel->AddSeparator();

           _goButton = panel->AddPushButton("Go",this);
//_jumptoframe->GlowWidget::SetRefreshEnabled(false);

		_jumptoframe->GlowTextFieldWidget::SetBlinkInterval(50000);


	 // new panel for X/Y/A display

	   	panel = vpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::horizontal);

     		 GlowLabeledPopupMenuWidget* popup = panel->AddPopupMenu(
		 		  "Morphogen:", this);
		                popup->AddItem("X");
 		                popup->AddItem("Y");
	 	                popup->AddItem("A"); 

	  // new panel for scale up/scale down buttons

		panel = vpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::vertical);
	


     _scaleslider = panel->AddSlider(0.1, 10.0, 1.0, GlowSliderWidget::logarithmic, 2, "%.1f", "SCALE: \n%.2f", this); 

         GlowQuickPanelWidget* subpanel = panel->AddPanel(
	        GlowQuickPanelWidget::transparentStyle, "",
		GlowQuickPanelWidget::horizontal);

	   _scaledownButton = subpanel->AddPushButton("<-Down",this);
 
           subpanel->AddSeparator(GlowSeparatorWidget::transparentStyle);
           subpanel->AddSeparator(GlowSeparatorWidget::transparentStyle);
           subpanel->AddSeparator(GlowSeparatorWidget::transparentStyle);

	   _scaleupButton = subpanel->AddPushButton("  Up->  ",this);

           panel->AddSeparator();

	   _resetButton = panel->AddPushButton("Reset",this);

		panel = vpanel->AddPanel(
		GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::vertical);


 	  _gizmoButton = panel->AddPushButton("New Glow Window", this);

	  _controlWindow->Pack();
}


// Destructor

DispButtons::~DispButtons()
{
	delete _controlWindow;
}



// Receive pushbutton events
void DispButtons::OnMessage(
	const GlowPushButtonMessage& message)
{
	GLOW_DEBUGSCOPE("DispButtons::OnMessage(pushbutton)");
	
	// Was it the quit button?
	if (message.widget == _quitButton)
	{
		exit(0);
	}
	// Was it the print button?
        else if (message.widget == _printButton)
	{
          glutSetWindow(gizmowindname);
	  //  glutSetWindow(_mainWindowID);
	  print_picture();
	}

	// Was it the Previous button?
        else if (message.widget == _previousButton)
	{
	  display_previous_picture();
	  Glow::RefreshGlutWindow(_mainWindowID);
          sprintf(jumpmsg,"%d",picturecounter);
          _jumptoframe->GlowTextFieldWidget::SetText(jumpmsg);
	}

	// Was it the Next button?
        else if (message.widget == _nextButton)
	{
          display_next_picture();
          cout << "Main Window" << _mainWindowID << "\n";
	  Glow::RefreshGlutWindow(_mainWindowID);
          sprintf(jumpmsg,"%d",picturecounter);
          _jumptoframe->GlowTextFieldWidget::SetText(jumpmsg);
          cout << "gizmowindow " << gizmowindname <<"\n";
	   Glow::RefreshGlutWindow(gizmowindname);

	  /* Following giving parse errors, 7/30/03 -         Glow::DeliverMouseDownEvt(,Glow::leftButton, int x, int y, Glow::noModifier);

	     DeliverMouseUpEvt(MandelGizmoSubwindow* _gizmoPanel, Glow::leftButton, int x, int y, Glow::noModifier); */

	  //	    _widgetsPanel->Refresh();

	  //_firstgizmowind->Refresh();
	  //  cout << "refreshed first gizmo wind\n";
	}

	// Was it the Go button?
        else if (message.widget == _goButton)
	{
	   sprintf(jumpmsg, "%s", _jumptoframe->GlowTextFieldWidget::GetTextChars());
           
           jumpnum = atoi(jumpmsg);

           jump_to_numbered_picture(jumpnum);
	 Glow::RefreshGlutWindow(_mainWindowID);
	}
	// Was it the Scale Down button?
        else if (message.widget == _scaledownButton)
	{
	  // want this to directly affect the value of the slider, then call the slider event, then 
	  // reset the scale_value. The following is a test, not the above method. 4/9/03
         scale_value/=1.1;
         _scaleslider->SetValue(scale_value);
	 Glow::RefreshGlutWindow(_mainWindowID);
	}

// Was it the Scale Up button?
        else if (message.widget == _scaleupButton)
	{
	  // want this to directly affect the value of the slider, then call the slider event, then 
	  // reset the scale_value. The following is a test, not the above method. 4/9/03
         scale_value*=1.1;
          _scaleslider->SetValue(scale_value);
	  Glow::RefreshGlutWindow(_mainWindowID);
	}

	// Was it the reset button?
	        else if (message.widget == _resetButton)
		{
		  //                  set_initial_scale();
		  scale_value = 1.0;
                  _scaleslider->SetValue(scale_value);
		  Glow::RefreshGlutWindow(_mainWindowID);
		  //	  _scaleslider::SetValue(5.0);
		}

// Gizmo button? (New with lesson 5)
	else if (message.widget == _gizmoButton)
	{
           gizmocounter++;
	   new MandelGizmoWindow(_image1, _image2, width, height);
	}     
}


// Receive popup menu events
void DispButtons::OnMessage(
	const GlowPopupMenuMessage& message)
{
	GLOW_DEBUGSCOPE("DispButtons::OnMessage(popupmenu)");

    // Only one popup menu was created-- assume it's the morphogen colour menu

	switch (message.item)
	{
		case 0:
			// Colour by X
		  mode = 1;
		  // display_grid();
		  Glow::RefreshGlutWindow(_mainWindowID);
			break;
		
		case 1:
			// Colour by Y
		   mode = 2;
                  //  display_grid();
		  Glow::RefreshGlutWindow(_mainWindowID);
			break;
		
		case 2:
			// Colour by A
 		    mode = 3;
                  //  display_grid();
		  Glow::RefreshGlutWindow(_mainWindowID);
			break;
	}
	//      }
	// else 
	//	{
	//  printf("didn't go into popup\n");
	//	}
}


// Receive checkbox events
void DispButtons::OnMessage(
	const GlowCheckBoxMessage& message)
{
	GLOW_DEBUGSCOPE("DispButtons::OnMessage(checkbox)");
	
	// Was it the toggle show grid checkbox?
	if (message.widget == _gridCheckBox)
	{
		if (message.state == GlowCheckBoxWidget::on)
		{
		  show_grid = TRUE;
	          Glow::RefreshGlutWindow(_mainWindowID);
		}
		else
		{
		  show_grid = FALSE;
	          Glow::RefreshGlutWindow(_mainWindowID);
		}
	}
        else
	{
           	if (message.state == GlowCheckBoxWidget::off)
		{
		  mode = 1;
	          Glow::RefreshGlutWindow(_mainWindowID);
		}
		else
		{
		  mode = 0;
	          Glow::RefreshGlutWindow(_mainWindowID);
		}
	}
}

// Receive Text Field events
/*void DispButtons::OnMessage( 
                   const GlowTextFieldWindowMessage& message )
{
	 GLOW_DEBUGSCOPE("DispButtons::OnMessage(textfield)");

	            printf("key is %s\n",key);


	 if(message.widget == _jumptoframe)
	 {
	
      sprintf(newmsg, "newmsg from field %s", _jumptoframe->GlowTextFieldWidget::GetTextChars());
              
    cout << "cout newmsg:" << newmsg << ", that was it\n";
	 }
	         else
	    {
              printf("key is %s\n",key);
	     } 
} */

// receive slider events

void DispButtons::OnMessage(
	    const GlowSliderMessage& message)
{
      GLOW_DEBUGSCOPE("DispButtons::OnMessage(slider)");
 
   // Was it the scale slider?

         if(message.widget == _scaleslider)
         {
            scale_value = message.value;
         }

    Glow::RefreshGlutWindow(_mainWindowID);
} 

/*void DispButtons::OnWidgetKeyboard(
                     Glow::KeyCode key, 
                     int x, 
                     int y, 
                     Glow::Modifiers modifiers)	
{
 if(key == Glow::enterKey)
    {
      cout << "Enter key pressed\n";
    }

}*/
