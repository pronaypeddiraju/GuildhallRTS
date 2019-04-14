//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math//AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include <vector>
#include <string>

struct InputEvent;
class UIRadioGroup;

//------------------------------------------------------------------------------------------------------------------------------
class UIWidget
{
public:
	UIWidget(UIWidget* parent); 
	virtual ~UIWidget(); // virtual dctr - why?  Baseline this should kill all my children; 

	void					UpdateBounds( AABB2 const &container ); 
	void					ProcessInput( InputEvent &evt ); // handles input - may consume the event (but it is still passed about to help update state)
	void					Render(); // assumes a camera has already been set; 

	void					SetColor(const Rgba& color);
	void					SetSize( const Vec4& size);
	void					SetPosition( const Vec4& position);

	UIWidget*				AddChild( UIWidget *widget ); 
	void					RemoveChild( UIWidget *widget ); 

	void					SetRadioGroup( UIRadioGroup *group ); // adds or removes me from a group

	// accessors
	inline Vec2				GetWorldPosition() const      { return m_position; }
	inline AABB2			GetWorldBounds() const       { return m_worldBounds; }

	AABB2					GetWidgetDimensions( const AABB2& parentBounds );

	Vec2					GetRelativePosToParent( const Vec2& positionInWorld );

public:
	// templated helper I like having
	template <typename T>
	T* CreateChild() 
	{
		// create a widget of type T, and automatically add it to my children; 
		// ...
		//IMPLEMENT_ME
	}

protected:
	// helpers; 
	void					UpdateChildrenBounds(); // will update children using my bounds as their container
	void					ProcessChildrenInput( InputEvent &evt ); // update input - process backwards through the list (things on top get first crack)
	void					RenderChildren();  // render children - process forward (later things show up on top)
	void					DestroyChildren(); // be sure to kill your children when you go... programming is dark; 

private:
	// heirarchy information
	UIWidget *m_parent         = nullptr; 
	std::vector<UIWidget*> m_children; 

	// human settable independent variables; 
	Vec4 m_virtualPosition     = Vec4( .5f, .5f, 0.0f, 0.0f );     // centered relative to container
	Vec4 m_virtualSize         = Vec4( 1.f, 1.f, 0.0f, 0.0f );   // stretch to 100% of the screen 
	Vec2 m_pivot               = Vec2( .5f, .5f );                 // centered

	// derived from the above;
	Vec2 m_position; 
	AABB2 m_worldBounds; 

	// Misc Data
	//eUIWidgetFlags m_uiFlags   = 0U;       // some state tracking; 
	UIRadioGroup *m_radioGroup = nullptr;  // which radio group do I belong to?

	Rgba m_color = Rgba::CLEAR;

	std::string	m_defaultShaderName	= "default_unlit.00.hlsl";
}; 

class UILabel : public UIWidget
{
	// implement me
	// ...
}; 


//typedef NamedProperties EventArgs;

class Event
{
public:
	Event();
	Event( std::string const &commandLine );

public:
	std::string m_name; 
	EventArgs m_args;
}; 

class UIButton : public UIWidget
{
	// implement me
	// ...

	/*
	void Click()
	{
		Event evt( m_eventClick ); 
		// m_name; 
		// m_args; 

		EventFire( evt.GetName(), evt.GetArgs() );  
	}
	*/

	std::string m_eventOnClick = "play map=level0.map";
}; 

class UISlider : public UIWidget
{
	/*
	void Change( float value ) 
	{
		m_value = value; 
		// whatever other updating needed for visual reasons;

		Event evt( m_eventOnChange ); 
		evt.m_args.Add( "value", m_value ); // add this AS A FLOAT
		evt.m_args.Add( "source", this );   //  who triggered this event; 

		EventFire( evt ); 
	}
	*/

	std::string m_eventOnChange = "changeRadius"; 
}; 

// A radio group allows for only a single item in the group 
// to be selected at a time - ie, mutually exclusive selection
class UIRadioGroup : public UIWidget
{
	// implement me
}; 