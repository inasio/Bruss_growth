/*
===============================================================================

	FILE:  mandelgizmo.h
	
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

#ifndef MANDELGIZMO__H
#define MANDELGIZMO__H


/*
===============================================================================
	Headers
===============================================================================
*/

#include "glow.h"
#include "glowQuickPalette.h"
#include "glowViewTransform.h"

GLOW_NAMESPACE_USING


/*
===============================================================================
	Gizmo toplevel window class
===============================================================================
*/

// class MandelGizmoSubwindow;
class MandelGizmoSubwindow :
	public GlowSubwindow
{
	public:
	
		MandelGizmoSubwindow(
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
                        GlowLabelWidget* morphmax);
	
	protected:
	
		virtual bool OnBeginPaint();
		virtual void OnEndPaint();
		//	virtual void OnMouseEnter();
		virtual void OnMouseDown(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
               virtual void OnMouseUp(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
		virtual void OnMouseDrag(
			int x,
			int y);
	
	private:
	
		GlowViewManipulator* _manip;
		GlowLabelWidget* _fpsIndicator;
		GlowLabelWidget* _dumpID;
		GlowLabelWidget* _area;
		GlowLabelWidget* _nodes;
		GlowLabelWidget* _zmax;
		GlowLabelWidget* _ctime;
		GlowLabelWidget* _morphmin;
		GlowLabelWidget* _morphmax;

		int _starting;
};

/*
===============================================================================
	Gizmo drawing component class
===============================================================================
*/

class MandelGizmoComponent :
	public GlowComponent
{
	public:
	
		MandelGizmoComponent(
			GlowComponent* parent,
			const unsigned char* image,
			int width,
			int height);
		~MandelGizmoComponent();
	
	protected:
	
		virtual void OnEndPaint();
	
	private:
	
		GLuint _texObject;
};


class MandelGizmoWindow :
	public GlowWindow
{
	public:
	
		MandelGizmoWindow(
			const unsigned char* image1, 
                        const unsigned char* image2,
			int width,
			int height);
	
		// 	GlowSubwindow* _colourPanel;
		MandelGizmoSubwindow* _gizmoPanel;
                MandelGizmoWindow*  _firstgizmowind;
	protected:
	
		virtual void OnReshape(
			int width,
			int height);

		/*      virtual void OnKeyboard(
                     Glow::KeyCode key, 
                     int x, 
                     int y, 
                     Glow::Modifiers modifiers);	*/

	
	private:
	
		GlowQuickPaletteSubwindow* _widgetsPanel;
		GlowQuickPaletteSubwindow* _textPanel;
};


#endif
