//------------------------------------------------------------------------------------------------------------------------------
#include "Game/UIWidget.hpp"
// Engine Systems
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
// Game systems
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
UIWidget::UIWidget(Game* game, UIWidget* parent)
{
	m_parent = parent;
	m_game = game;
	m_font = g_renderContext->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");
}

//------------------------------------------------------------------------------------------------------------------------------
UIWidget::~UIWidget()
{
	m_parent = nullptr;
	DestroyChildren();
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::UpdateBounds( AABB2 const &container )
{
	m_worldBounds = GetWidgetDimensions(container);
	//m_worldBounds = container;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::ProcessInput( InputEvent &evt )
{
	ProcessChildrenInput(evt);

	IntVec2 mousePos = g_windowContext->GetClientMousePosition();
	IntVec2 clientSize = g_windowContext->GetTureClientBounds();

	Vec2 relativePosition;
	relativePosition.x = RangeMapFloat((float)mousePos.x, 0.f, (float)clientSize.x, -UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * 0.5f, UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * 0.5f);
	relativePosition.y = RangeMapFloat((float)mousePos.y, 0.f, (float)clientSize.y, UI_SCREEN_HEIGHT * 0.5f, -UI_SCREEN_HEIGHT * 0.5f);

	AABB2 BoxDimensions = GetWidgetDimensions(AABB2(Vec2::ZERO, Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT)));
	AABB2 boxDimensions(GetRelativePosToParent(BoxDimensions.GetBottomLeft2D()), GetRelativePosToParent(BoxDimensions.GetTopRight2D()));

	if( IsPointInAABBB2(boxDimensions, relativePosition))
	{
		UIButton* button = dynamic_cast<UIButton*>(this);

		if(button != nullptr && !evt.m_consumed)
		{
			std::string key = "clickType";
			evt.m_clickType = evt.m_args.GetValue(key, "");
			if(evt.m_clickType == "LBDown")
			{
				button->Click();
				evt.m_consumed = true;
			}
		}
	}
	
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::Render()
{
	g_renderContext->BindShader(g_renderContext->CreateOrGetShaderFromFile(m_defaultShaderName));
	m_font = g_renderContext->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");

	g_renderContext->BindTextureViewWithSampler(0U, nullptr);
	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);

	//draw this widget in the screen using the position, anchor and pivot
	if(m_parent == nullptr)
	{
		//We are the parent to all, draw the screen
		AABB2 parentBounds = m_worldBounds;
		AABB2 BoxDimensions = GetWidgetDimensions(parentBounds);

		std::vector<Vertex_PCU> boxVerts;
		AABB2 boxDimensions(GetRelativePosToParent(BoxDimensions.GetBottomLeft2D()), GetRelativePosToParent(BoxDimensions.GetTopRight2D()));
		AddVertsForAABB2D(boxVerts, boxDimensions, m_color);

		g_renderContext->DrawVertexArray(boxVerts);

		RenderMouseData();
	}
	else
	{
		//We have a parent, render for different widget types
		RenderForWidgetType();
	}

	//Render my children here
	RenderChildren();
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::RenderForWidgetType()
{
	switch( m_widgetType )
	{
	case UI_BUTTON:
	{
		UIButton* button = dynamic_cast<UIButton*>(this);

		AABB2 parentBounds = m_parent->GetWorldBounds();
		AABB2 BoxDimensions = GetWidgetDimensions(parentBounds);

		std::vector<Vertex_PCU> boxVerts;
		AABB2 boxDimensions(GetRelativePosToParent(BoxDimensions.GetBottomLeft2D()), GetRelativePosToParent(BoxDimensions.GetTopRight2D()));
		
		g_renderContext->SetModelMatrix(Matrix44::IDENTITY);

		//Check if there is hover (Find a better place to check this! Have an update)
		IntVec2 mousePos = g_windowContext->GetClientMousePosition();
		IntVec2 clientSize = g_windowContext->GetTureClientBounds();

		Vec2 relativePosition;
		relativePosition.x = RangeMapFloat((float)mousePos.x, 0.f, (float)clientSize.x, -UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * 0.5f, UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * 0.5f);
		relativePosition.y = RangeMapFloat((float)mousePos.y, 0.f, (float)clientSize.y, UI_SCREEN_HEIGHT * 0.5f, -UI_SCREEN_HEIGHT * 0.5f);

		Rgba color = m_color;
		UIButton* button2 = dynamic_cast<UIButton*>(this);

		//Add verts for the box
		if(button->m_buttonTexture != nullptr)
		{
			g_renderContext->BindTextureViewWithSampler(0U, button->m_buttonTexture);
			AddVertsForAABB2D(boxVerts, boxDimensions, color);
			g_renderContext->DrawVertexArray(boxVerts);
		}
		else
		{
			g_renderContext->BindTextureViewWithSampler(0U, nullptr);
			color = Rgba::CLEAR;
			AddVertsForAABB2D(boxVerts, boxDimensions, color);
			g_renderContext->DrawVertexArray(boxVerts);
		}

		if( IsPointInAABBB2(boxDimensions, relativePosition))
		{
			button2->OnHover();
			color = m_color;

			boxVerts.clear();
			g_renderContext->BindTextureViewWithSampler(0U, nullptr);
			AddVertsForBoundingBox(boxVerts, boxDimensions, color, 0.05f * boxDimensions.GetWidth());
			g_renderContext->DrawVertexArray(boxVerts);
		}
		else
		{
			button2->OnUnHover();
			color = m_color;

			boxVerts.clear();
			g_renderContext->BindTextureViewWithSampler(0U, nullptr);
			AddVertsForBoundingBox(boxVerts, boxDimensions, color, 0.05f * boxDimensions.GetWidth());
			g_renderContext->DrawVertexArray(boxVerts);
		}

	}
	break;
	case UI_LABEL:
	{
		UILabel* label = dynamic_cast<UILabel*>(this);

		AABB2 parentBounds = m_parent->GetWorldBounds();
		AABB2 BoxDimensions = GetWidgetDimensions(parentBounds);

		//Check if the parent is a button, if yes set the parent's color as label color
		UIButton* button = dynamic_cast<UIButton*>(this->m_parent);
		
		if(button != nullptr)
		{
			m_color = button->unHovercolor;
		}

		std::vector<Vertex_PCU> textVerts;
		AABB2 boxDimensions(GetRelativePosToParent(BoxDimensions.GetBottomLeft2D()), GetRelativePosToParent(BoxDimensions.GetTopRight2D()));
		m_font->AddVertsForTextInBox2D(textVerts, boxDimensions, BoxDimensions.GetHeight() * label->m_virtualSize.y, label->m_labelText, label->m_color);

		g_renderContext->BindTextureViewWithSampler(0U, m_font->GetTexture());
		g_renderContext->SetModelMatrix(Matrix44::IDENTITY);

		g_renderContext->DrawVertexArray(textVerts);
	}
	break;
	default:
	{
		/*
		AABB2 parentBounds = m_parent->GetWorldBounds();
		AABB2 BoxDimensions = GetWidgetDimensions(parentBounds);

		std::vector<Vertex_PCU> boxVerts;
		AABB2 boxDimensions(GetRelativePosToParent(BoxDimensions.GetBottomLeft2D()), GetRelativePosToParent(BoxDimensions.GetTopRight2D()));
		AddVertsForAABB2D(boxVerts, boxDimensions, m_color);

		g_renderContext->BindTextureViewWithSampler(0U, nullptr);
		g_renderContext->SetModelMatrix(Matrix44::IDENTITY);

		g_renderContext->DrawVertexArray(boxVerts);
		*/
	}
	break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::RenderMouseData() const
{
	//Print the mouse position
	IntVec2 mousePos = g_windowContext->GetClientMousePosition();

	Vec2 correctedPos;
	IntVec2 CamSize = g_renderContext->GetCurrentScreenDimensions();

	AABB2 parentBounds;
	if(m_parent != nullptr)				{	parentBounds = m_parent->GetWorldBounds();												   }
	else 								{	parentBounds = AABB2(Vec2::ZERO, Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT));}

	AABB2 CamBounds = AABB2(Vec2(CamSize.x * -0.5f, CamSize.y * -0.5f), Vec2(CamSize.x * 0.5f, CamSize.y * 0.5f));

	correctedPos.x = RangeMapFloat((float)mousePos.x, parentBounds.m_minBounds.x, parentBounds.m_maxBounds.x, CamBounds.m_minBounds.x, CamBounds.m_maxBounds.x);
	correctedPos.y = RangeMapFloat((float)mousePos.y, parentBounds.m_minBounds.y, parentBounds.m_maxBounds.y, CamBounds.m_minBounds.y, CamBounds.m_maxBounds.y);



}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::SetColor( const Rgba& color )
{
	m_color = color;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::SetSize( const Vec4& size )
{
	m_virtualSize = size;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::SetPosition( const Vec4& position )
{
	m_virtualPosition = position;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::SetPivot( const Vec2& pivoit )
{
	m_pivot = pivoit;
}

//------------------------------------------------------------------------------------------------------------------------------
UIWidget* UIWidget::AddChild( UIWidget *widget )
{
	m_children.push_back(widget);
	return widget;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::RemoveChild( UIWidget *widget )
{
	for(int widgetIndex = 0; widgetIndex < (int)m_children.size(); widgetIndex++)
	{
		if(m_children[widgetIndex] == widget)
		{
			delete m_children[widgetIndex];
			m_children[widgetIndex] = nullptr;
			m_children.erase(m_children.begin() + widgetIndex);
			break;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::SetWidgetType( eWidgetType widgetType )
{
	m_widgetType = widgetType;
}

//------------------------------------------------------------------------------------------------------------------------------
AABB2 UIWidget::GetWidgetDimensions( const AABB2& parentBounds )
{
	float parentWidth = parentBounds.GetWidth();
	float parentHeight = parentBounds.GetHeight();

	//Get the size of the local box
	Vec2 BL = Vec2::ZERO;
	Vec2 TR = Vec2(parentWidth * m_virtualSize.x, parentHeight * m_virtualSize.y);

	AABB2 relativeBounds(GetRelativePosToParent(parentBounds.m_minBounds), GetRelativePosToParent(parentBounds.m_maxBounds));

	//Move it to the correct position
	Vec2 center = parentBounds.GetBottomLeft2D() + Vec2(m_virtualPosition.x * parentWidth, m_virtualPosition.y * parentHeight);
	m_position = center;
	BL = center + Vec2(parentWidth * m_virtualSize.x, parentHeight * m_virtualSize.y)  * -1.f * m_pivot;
	TR = center + Vec2(parentWidth * m_virtualSize.x, parentHeight * m_virtualSize.y) * 1.f * m_pivot;

	//Add any offsets
	BL += Vec2(m_virtualPosition.z, m_virtualPosition.w);
	TR += Vec2(m_virtualPosition.z, m_virtualPosition.w);

	AABB2 box = AABB2(BL, TR);

	return box;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 UIWidget::GetRelativePosToParent( const Vec2& positionInWorld )
{
	Vec2 correctedPos;
	IntVec2 CamSize = g_renderContext->GetCurrentScreenDimensions();
	
	AABB2 parentBounds;
	/*
	if(m_parent != nullptr)				{	parentBounds = m_parent->GetWorldBounds();												   }
	else 								{	parentBounds = AABB2(Vec2::ZERO, Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT));}
	*/

	parentBounds = AABB2(Vec2::ZERO, Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT));

	AABB2 CamBounds = AABB2(Vec2(CamSize.x * -0.5f, CamSize.y * -0.5f), Vec2(CamSize.x * 0.5f, CamSize.y * 0.5f));

	correctedPos.x = RangeMapFloat(positionInWorld.x, parentBounds.m_minBounds.x, parentBounds.m_maxBounds.x, CamBounds.m_minBounds.x, CamBounds.m_maxBounds.x);
	correctedPos.y = RangeMapFloat(positionInWorld.y, parentBounds.m_minBounds.y, parentBounds.m_maxBounds.y, CamBounds.m_minBounds.y, CamBounds.m_maxBounds.y);
	return correctedPos;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::UpdateChildrenBounds()
{
	for(int widgetIndex = 0; widgetIndex < (int)m_children.size(); widgetIndex++)
	{
		m_children[widgetIndex]->UpdateBounds(m_worldBounds);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::ProcessChildrenInput( InputEvent &evt )
{
	if(m_children.size() > 0U)
	{
		for (int childIndex = (int)m_children.size() - 1; childIndex >= 0; childIndex--)
		{
			m_children[childIndex]->ProcessInput(evt);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::RenderChildren()
{
	for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
	{
		m_children[childIndex]->Render();
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::DestroyChildren()
{
	for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
	{
		delete m_children[childIndex];
		m_children[childIndex] = nullptr;
		m_children.erase(m_children.begin() + childIndex);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
InputEvent::InputEvent( const std::string& name, const EventArgs& args )
{
	m_name = name;
	m_args = args;
	m_clickType = args.GetValue("clickType", "");	
}

//------------------------------------------------------------------------------------------------------------------------------
InputEvent::~InputEvent()
{

}

//------------------------------------------------------------------------------------------------------------------------------
Event::Event( std::string const &commandLine )
{
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(commandLine, ' ');

	m_name = splitStrings[0];

	for(int stringIndex = 1; stringIndex < static_cast<int>(splitStrings.size()); stringIndex++)
	{
		//split on =
		std::vector<std::string> KeyValSplit = SplitStringOnDelimiter(splitStrings[stringIndex], '=');
		if(KeyValSplit.size() != 2)
		{
			continue;
		}
		else
		{
			m_args.SetValue(KeyValSplit[0], KeyValSplit[1]);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
Event::~Event()
{

}

//------------------------------------------------------------------------------------------------------------------------------
UIButton::UIButton( Game* game, UIWidget* parent )
	: UIWidget(game, parent)
{
	SetWidgetType(UI_BUTTON);
}

//------------------------------------------------------------------------------------------------------------------------------
UIButton::~UIButton()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void UIButton::SetOnClick( const std::string& onClickEvent )
{
	m_eventOnClick = onClickEvent;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIButton::SetButtonTexture( const std::string& texturePath )
{
	m_buttonTexture = g_renderContext->GetOrCreateTextureViewFromFile(texturePath);
}

//------------------------------------------------------------------------------------------------------------------------------
void UIButton::SetOnHover( const std::string& onHoverEvent )
{
	m_eventOnHover = onHoverEvent;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIButton::SetRadioType( bool radioType )
{
	m_isRadioType = radioType;
}

//------------------------------------------------------------------------------------------------------------------------------
UILabel::UILabel( Game* game, UIWidget* parent )
	: UIWidget(game, parent)
{
	SetWidgetType(UI_LABEL);
}

//------------------------------------------------------------------------------------------------------------------------------
UILabel::~UILabel()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void UILabel::SetLabelText( const std::string& labelText )
{
	m_labelText = labelText;
}

//------------------------------------------------------------------------------------------------------------------------------
UIRadioGroup::UIRadioGroup(Game* game, UIWidget* parent)
	: UIWidget(game, parent)
{
	SetWidgetType(UI_RADIO_GROUP);
}

//------------------------------------------------------------------------------------------------------------------------------
UIRadioGroup::~UIRadioGroup()
{

}
