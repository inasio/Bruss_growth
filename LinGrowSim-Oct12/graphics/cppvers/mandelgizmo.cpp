/*
===============================================================================

	FILE:  mandelgizmo.cpp
	
	PROJECT:
	
		GLOW-based mandelbrot viewer
	
	CONTENTS:
	
		Gizmo window
	
	PROGRAMMERS:
	
		Daniel Azuma <dazuma@kagi.com>
	
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

#include "mandelgizmo.h"
#include "types.h"
#include "display.h"
#include "globals.h"

int gizmowindname, gizmosubwindname;

char dumpmsg[80], areamsg[80], nodesmsg[80], zmaxmsg[80], msg[80];
char morphminmsg[80], morphmaxmsg[80];


/*
===============================================================================
	Methods of MandelGizmoComponent
===============================================================================
*/

// Constructor

MandelGizmoComponent::MandelGizmoComponent(
	GlowComponent* parent,
	const unsigned char* image,
	int width,
	int height) :
GlowComponent(parent)
{
	GLOW_DEBUGSCOPE("MandelGizmoComponent::MandelGizmoComponent");
}


// Destructor

MandelGizmoComponent::~MandelGizmoComponent()
{
	GLOW_DEBUGSCOPE("MandelGizmoComponent::~MandelGizmoComponent");
}


// Redraw method

void MandelGizmoComponent::OnEndPaint()
{
	GLOW_DEBUGSCOPE("MandelGizmoComponent::OnEndPaint");

	//	give_color_bar(common_rgbmap);
	//       give_text_and_bar();

	// Unlike in, e.g. display_previous_picture, display_grid
	// is needed here, rather than init_triangles. None of
	// it seems to have anything to do with Refresh of 
	// MandelGizmo(Sub)window.

        //init_triangles();
	display_grid();
}


/*
===============================================================================
	Methods of MandelGizmoSubwindow
===============================================================================
*/

// Constructor

MandelGizmoSubwindow::MandelGizmoSubwindow(
	GlowComponent* parent,
	int size,
	int ypos,
	const unsigned char* image,
	int width,
	int height,
        GlowLabelWidget* fpsIndicator,
	GlowLabelWidget* dumpID,
	GlowLabelWidget* area,
	GlowLabelWidget* nodes,
	GlowLabelWidget* zmax,
        GlowLabelWidget* ctime,
        GlowLabelWidget* morphmin,
        GlowLabelWidget* morphmax) :

// Intialize subwindow
GlowSubwindow(parent, 0, ypos, size, size, Glow::rgbaBuffer | Glow::depthBuffer |
	Glow::doubleBuffer, Glow::mouseEvents | Glow::dragEvents)
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::MandelGizmoSubwindow");
	
	// Don't automatically swap buffers
	SetAutoSwapBuffersEnabled(false);
	
	// Attach maniuplator to subwindow
	_manip = new GlowViewManipulator(this,
		GlowViewManipulatorParams::defaults);
	_manip->SetSpinnable(true);
	
	// Attach drawing component to manipulator
	new MandelGizmoComponent(_manip, image, width, height);
	
	// Store away this pointer
	_fpsIndicator = fpsIndicator;
        _dumpID = dumpID;
        _area = area;
        _nodes = nodes;
        _zmax = zmax;
        _ctime = ctime;
        _morphmin = morphmin;
        _morphmax = morphmax;
}


// Begin-redraw method

bool MandelGizmoSubwindow::OnBeginPaint()
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnBeginPaint");
	
	// Begin computing frame rate.
	_starting = Glow::GetMilliseconds();
	
	// Clear buffer - redundant? Glow default clears to black.
		::glClearColor(0.0, 0.0, 0.0, 0.0);
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Set up transforms
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	//::glFrustum(-0.5, 0.5, -0.5, 0.5, 1.0, 8.0);
	::glOrtho(-4.0,4.0,-4.0,4.0,-100.0,100.0);
	::glMatrixMode(GL_MODELVIEW);
	

	//	::glDisable(GL_NORMALIZE);
	::glEnable(GL_DEPTH_TEST);
	//	::glEnable(GL_CULL_FACE);
	glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); /*doesn't seem to matter */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDisable(GL_BLEND); 
	//       glClearColor(0.0, 0.0, 1.0, 0.0);
        glShadeModel(GL_FLAT);
        glClearDepth(1.0);    
	
	// Offset camera
	::glTranslatef(0.0f, 0.0f, -4.0f);
	
	return true;
}


// End-redraw method

void MandelGizmoSubwindow::OnEndPaint()
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnEndPaint");
         
       
	give_color_bar(common_rgbmap);

	// Flush the pipeline
		::glFlush();

          Glow::SwapBuffers();
	
	//insert call to display_grid here, 4/16/03

	//      display_grid();
	//	Glow::SwapBuffers();;

	// Finish computing frame rate
	char buf[100];
	sprintf(buf, "%.2f fps",
		float(1000)/float(Glow::GetMilliseconds()-_starting));
	_fpsIndicator->SetText(buf);
        
	//	char dumpmsg[80];
         sprintf(dumpmsg,"Frame %d",picturecounter);
        _dumpID->SetText(dumpmsg);

         sprintf(areamsg,"Area is  %.2f", the_area);
        _area->SetText(areamsg);

         sprintf(nodesmsg,"%d Nodes", number_of_nodes );
        _nodes->SetText(nodesmsg);

         sprintf(zmaxmsg,"Max Height is  %.2f", max_z);
        _zmax->SetText(zmaxmsg);

	sprintf(msg, "Time %.1f", iterations*delta_t);
        _ctime->SetText(msg);

	sprintf(morphminmsg, "X Min is %.3f", min_X_value );
        _morphmin->SetText(morphminmsg);

	sprintf(morphmaxmsg, "X Max is %.3f", max_X_value);
        _morphmax->SetText(morphmaxmsg);
}

/*void MandelGizmoSubwindow::OnMouseEnter(
                                   )
{
    if (_manip->GetState() == GlowViewManipulator::idleState)
	{
  init_triangles();
  display_grid();
  Refresh();
  ::glFlush();
  } 
} */

// Respond to mouse down events

void MandelGizmoSubwindow::OnMouseDown(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnMouseDown");
	
	// If the manipulator is not yet active, then begin rotating
	if (_manip->GetState() == GlowViewManipulator::idleState)
	{
		// Note that we need to transform window coordinates to
		// OpenGL coordinates.
		GLfloat xn, yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->BeginRotationDrag(xn, yn);
	}
}


// Respond to mouse up events

void MandelGizmoSubwindow::OnMouseUp(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnMouseUp");
	
	// If the manipulator is currently rotating, then finish rotating
	if (_manip->GetState() == GlowViewManipulator::rotatingState)
	{
		GLfloat xn, yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->EndDrag(xn, yn);
	}
}


// Respond to mouse drag events

void MandelGizmoSubwindow::OnMouseDrag(
	int x,
	int y)
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnMouseDrag");
	
	// If the manipulator is currently rotating, then pass drag event
	// on to the manipulator
	if (_manip->GetState() == GlowViewManipulator::rotatingState)
	{
		GLfloat xn, yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->InDrag(xn, yn);
	}
}


/*
===============================================================================
	Methods of MandelGizmoWindow
===============================================================================
*/

// Constructor

MandelGizmoWindow::MandelGizmoWindow(
	const unsigned char* image1,
	const unsigned char* image2,
	int width,
	int height)
{
	GLOW_DEBUGSCOPE("MandelGizmoWindow::MandelGizmoWindow");
	
	// Initialize window

	//	char dumpmsg[80];

	if(gizmocounter==0)
	{
          width = 500;
          height = 500;
	  Init("Growth: 1st Glow Window", 250, 200,
		width, height, Glow::rgbBuffer, Glow::allEvents);
	  gizmowindname=GlutWindowNum(); 

	  cout << "gizmowindname is " << gizmowindname << "\n";
	  Refresh();

	  //	  Glow::SetIdleFunc(&MandelGizmoWindow::Refresh);
	}
        else
	{
	  width=400;
          height=400;
 	   Init("Growth: Popup Glow Window", GlowWindow::autoPosition, GlowWindow::autoPosition, width, height, Glow::rgbBuffer, Glow::allEvents);
	}

	// Create widget panel with close button and frame rate indicator.
	// Note that we use a margin of 0
	_widgetsPanel = new GlowQuickPaletteSubwindow(this, 0, 0, width, 25,
		GlowQuickPalette::horizontal, GlowQuickPalette::alignLeft, 10, 5, 5);

        GlowQuickPanelWidget* vpanel1 = _widgetsPanel->AddArrangingPanel(
		GlowQuickPanelWidget::vertical);
        vpanel1->SetBackColor(GlowColor::white);
	vpanel1->AddDismissPushButton("Close", this);
	// Note that we provide initial text for the label widget so its
	// computed width is wide enough to display any fps we want.
	GlowLabelWidget* fpsIndicator = vpanel1->AddLabel("00.00 fps");
	GlowLabelWidget* xaxisIndicator = vpanel1->AddLabel("X-axis: Red");
        xaxisIndicator->SetTextColor(GlowColor::red);
        GlowLabelWidget* yaxisIndicator = vpanel1->AddLabel("Y-axis: Green"); 
        yaxisIndicator->SetTextColor(GlowColor::green);
        GlowLabelWidget* zaxisIndicator = vpanel1->AddLabel("Z-axis: Blue");
        zaxisIndicator->SetTextColor(GlowColor::blue);
	
        GlowQuickPanelWidget* vpanel2 = _widgetsPanel->AddArrangingPanel(
		GlowQuickPanelWidget::vertical);
	
	// First we have a few general controls

           GlowQuickPanelWidget* panel = vpanel2->AddPanel(
	        GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::horizontal);
	// Add a new panel that will contain text - Glow seems easier for
	// text than straight OpenGl.

       GlowLabelWidget* dumpID = panel->AddLabel("Frame 000");


        panel->AddSeparator();
 
       GlowLabelWidget* ctime = panel->AddLabel("Time is 00.0");

// Another  panel for zmax, nodes, etc.

	/*	_textPanel = new GlowQuickPaletteSubwindow(this, 0, 25, width, 25,
		GlowQuickPalette::horizontal, GlowQuickPalette::alignCenter, 10, 0, 0); */

	//       panel->AddSeparator();
          
                panel = vpanel2->AddPanel(
	        GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::horizontal);

        GlowLabelWidget* zmax = panel->AddLabel("Max Height is 00.000");
  
	   //        _widgetsPanel->AddSeparator();
 	//       	_widgetsPanel->AddLabel(dumpmsg);

        panel->AddSeparator();
	
       GlowLabelWidget* nodes = panel->AddLabel("000,000 Nodes");
	
        panel->AddSeparator();

       GlowLabelWidget* area = panel->AddLabel("Area is 00.000");

       // new sub-panel for Xmin, Xmax 

                panel = vpanel2->AddPanel(
	        GlowQuickPanelWidget::loweredStyle, "",
		GlowQuickPanelWidget::horizontal);

           GlowLabelWidget* morphmin = panel->AddLabel("X Minimum is 0.000");
  
	   //        _widgetsPanel->AddSeparator();
 	//       	_widgetsPanel->AddLabel(dumpmsg);

        panel->AddSeparator();
	
	 GlowLabelWidget* morphmax = panel->AddLabel("X Maximum is 0.000");

	_widgetsPanel->Pack(true);

	/*_colourPanel = new GlowSubwindow(this, 0, _widgetsPanel->Height(), width, 200, Glow::rgbBuffer, Glow::noEvents);
       
	 glClearColor(1.0, 1.0, 1.0, 0.0);
	 glClear(GL_COLOR_BUFFER_BIT); 

          

	//       give_color_bar(common_rgbmap);

	// Flush the pipeline
	      glFlush();    
	     //   Glow::SwapBuffers();
	     Refresh();*/
       

    //_colourPanel = new MandelGizmoSubwindow(this, width, _widgetsPanel->Height(), image1, width, height, fpsIndicator, dumpID, area, nodes, zmax, ctime, morphmin, morphmax);

	//Reshape(width, height+_widgetsPanel->Height());


	//   Create the subwindow that will contain the main plant image

        _gizmoPanel = new MandelGizmoSubwindow(this, width, _widgetsPanel->Height()+height, image2, width, height, fpsIndicator, dumpID, area, nodes, zmax, ctime, morphmin, morphmax);

	    Reshape(width, height+_widgetsPanel->Height());
	  gizmosubwindname=GlutWindowNum();
          cout << "subwindname is" << gizmosubwindname << "\n"; 

}


// Respond to reshape events

void MandelGizmoWindow::OnReshape(
	int width,
	int height)
{
	GLOW_DEBUGSCOPE("MandelGizmoWindow::OnReshape");
	
	// Enforce minimum size
	if (width < 40 || height < 40+_widgetsPanel->Height())
	{
		Reshape(max(width, 40), max(height, 40+_widgetsPanel->Height()));
	}
	else
	{
		// Reshape the widget panel if the width has changed
		if (width != _widgetsPanel->Width())
		{
			_widgetsPanel->Reshape(width, _widgetsPanel->Height());
		}
		
		// Position and size the drawing subwindow.
		height -= _widgetsPanel->Height();
		int gizmoSize = min(width, height);
		_gizmoPanel->Move((width-gizmoSize)/2,
			_widgetsPanel->Height()+(height-gizmoSize)/2);
		_gizmoPanel->Reshape(gizmoSize, gizmoSize);
	}
	
	// Update the viewport
	::glViewport(0, 0, width, height);
}
/*void MandelGizmoWindow::OnKeyboard(
                     Glow::KeyCode key, 
                     int x, 
                     int y, 
                     Glow::Modifiers modifiers)	
{
 if(key == Glow::enterKey)
    {
      printf("enter key pressed\n");
      cout << "Enter key pressed\n";
    }

}*/
