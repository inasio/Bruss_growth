/*
===============================================================================

	FILE:  dispbuttons.h
	
	PROJECT:
	
		GLOW widgets for display_w_buttons.c
	
	CONTENTS:
	
		Controls window
	
	PROGRAMMERS:
	
		DH, after Daniel Azuma <dazuma@kagi.com>
	
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

#ifndef DISPCONTROLS__H
#define DISPCONTROLS__H


/*
===============================================================================
	Headers
===============================================================================
*/

#include "glow.h"
#include "glowQuickPalette.h"

GLOW_NAMESPACE_USING


/*
===============================================================================
	Controls class
===============================================================================
*/

// MechControls is an object that receives a number of events.
// The idle event receiver is used for background calculating.
// The TextFieldWindow event receiver is used for the save image dialog.
// The other event receivers are widgets in the control palette.

class DispButtons :
      public GlowPushButtonReceiver,
      public GlowPopupMenuReceiver,
      public GlowSliderReceiver,
      public GlowCheckBoxReceiver
{
	public:
  
		DispButtons(
			int mainWindowID);
		virtual ~DispButtons();
	
	protected:
	
		// Widget events
		
		// Implement this method to receive pushbutton events
		virtual void OnMessage(
			const GlowPushButtonMessage& message);

		// method to receive popup events
                virtual void OnMessage(
			  const GlowPopupMenuMessage& message);

		// method to receive check box events
		virtual void OnMessage(
		    const GlowCheckBoxMessage& message);

		// method to receive slider events
		virtual void OnMessage(
		    const GlowSliderMessage& message);

		/*               virtual void OnKeyboard(
                     Glow::KeyCode key, 
                     int x, 
                     int y, 
                     Glow::Modifiers modifiers);	*/

		// method to receive text field events
		/*	virtual void OnMessage(
			const GlowTextFieldWindowMessage& message); */


	private:
	
		// Main drawing window id
                   int _mainWindowID;
		
		// Pointer to the control window itself
 		GlowQuickPaletteWindow* _controlWindow;
		//	       	GlowQuickPaletteWindow* _conWin2;
		
		// Pointers to selected widgets. We store these so that we can tell
		// what widget was hit when we receive events.

                      GlowCheckBoxWidget* _gridCheckBox;
                      GlowSliderWidget* _scaleslider;

		      GlowPushButtonWidget* _quitButton;
                      GlowPushButtonWidget* _printButton;
                      GlowPushButtonWidget* _previousButton;
                      GlowPushButtonWidget* _nextButton;
                      GlowPushButtonWidget* _goButton;
	              GlowPushButtonWidget* _resetButton;
		      GlowPushButtonWidget* _scaledownButton;
		      GlowPushButtonWidget* _scaleupButton;
                      GlowLabeledTextFieldWidget* _jumptoframe;
};


#endif
