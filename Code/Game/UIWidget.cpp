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
#include "Game/GameCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
UIWidget::UIWidget(UIWidget* parent)
{
	m_parent = parent;
}

//------------------------------------------------------------------------------------------------------------------------------
UIWidget::~UIWidget()
{
	m_parent = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::UpdateBounds( AABB2 const &container )
{
	m_worldBounds = GetWidgetDimensions(container);
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::ProcessInput( InputEvent &evt )
{
	ProcessChildrenInput(evt);

	if(m_parent == nullptr)
	{
		return;
	}

	IntVec2 mousePos = g_windowContext->GetClientMousePosition();
	IntVec2 clientSize = g_windowContext->GetTureClientBounds();

	Vec2 relativePosition;
	relativePosition.x = RangeMapFloat(mousePos.x, 0.f, clientSize.x, -UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * 0.5f, UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * 0.5f);
	relativePosition.y = RangeMapFloat(mousePos.y, 0.f, clientSize.y, UI_SCREEN_HEIGHT * 0.5f, -UI_SCREEN_HEIGHT * 0.5f);

	AABB2 BoxDimensions = GetWidgetDimensions(AABB2(Vec2::ZERO, Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT)));
	AABB2 boxDimensions(GetRelativePosToParent(BoxDimensions.GetBottomLeft2D()), GetRelativePosToParent(BoxDimensions.GetTopRight2D()));

	if( IsPointInAABBB2(boxDimensions, relativePosition))
	{
		g_devConsole->PrintString(m_color, "Is in button");
	}
	else
	{

	}
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::Render()
{
	g_renderContext->BindShader(g_renderContext->CreateOrGetShaderFromFile(m_defaultShaderName));
	BitmapFont* font = g_renderContext->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");

	//Get the mouse position in client space
	IntVec2 mousePos = g_windowContext->GetClientMousePosition();
	IntVec2 clientSize = g_windowContext->GetTureClientBounds();

	//Cursor Relative Position in UI space
	Vec2 relativePosition;
	relativePosition.x = RangeMapFloat(mousePos.x, 0.f, clientSize.x, -UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * 0.5f, UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * 0.5f);
	relativePosition.y = RangeMapFloat(mousePos.y, 0.f, clientSize.y, UI_SCREEN_HEIGHT * 0.5f, -UI_SCREEN_HEIGHT * 0.5f);

	g_renderContext->BindTextureViewWithSampler(0U, nullptr);
	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);

	//draw this widget in the screen using the position, anchor and pivot
	if(m_parent == nullptr)
	{
		//We are the parent to all, draw the screen
		//IntVec2 screenDimensions = g_renderContext->GetCurrentScreenDimensions();
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
		//we have a parent UI Widget

		AABB2 parentBounds = m_parent->GetWorldBounds();
		AABB2 BoxDimensions = GetWidgetDimensions(parentBounds);

		std::vector<Vertex_PCU> boxVerts;
		AABB2 boxDimensions(GetRelativePosToParent(BoxDimensions.GetBottomLeft2D()), GetRelativePosToParent(BoxDimensions.GetTopRight2D()));
		AddVertsForAABB2D(boxVerts, boxDimensions, m_color);

		g_renderContext->DrawVertexArray(boxVerts);

		if( IsPointInAABBB2(boxDimensions, relativePosition))
		{
			std::vector<Vertex_PCU> boxTextVerts;
			std::string textValue = "Max: " + std::to_string(boxDimensions.m_maxBounds.x) + ", " + std::to_string(boxDimensions.m_maxBounds.y);
			font->AddVertsForText2D(boxTextVerts, boxDimensions.m_maxBounds, 10.f, textValue);

			textValue = "Min: " + std::to_string(boxDimensions.m_minBounds.x) + ", " + std::to_string(boxDimensions.m_minBounds.y);
			font->AddVertsForText2D(boxTextVerts, boxDimensions.m_minBounds, 10.f, textValue);

			g_renderContext->BindTextureView(0U, font->GetTexture());
			g_renderContext->DrawVertexArray(boxTextVerts);
		}

	}

	//Render my children here
	RenderChildren();
	
	std::vector<Vertex_PCU> textVerts;
	std::string textValue = "Relative Pos: " + std::to_string(relativePosition.x) + ", " + std::to_string(relativePosition.y);
	font->AddVertsForText2D(textVerts, Vec2(0.0f, 0.f), 10.f, textValue);
	std::string textValue2 = "Client Pos: " + std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y);
	font->AddVertsForText2D(textVerts, Vec2(0.0f, 10.f), 10.f, textValue2);

	g_renderContext->BindTextureView(0U, font->GetTexture());
	g_renderContext->DrawVertexArray(textVerts);
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

	correctedPos.x = RangeMapFloat(mousePos.x, parentBounds.m_minBounds.x, parentBounds.m_maxBounds.x, CamBounds.m_minBounds.x, CamBounds.m_maxBounds.x);
	correctedPos.y = RangeMapFloat(mousePos.y, parentBounds.m_minBounds.y, parentBounds.m_maxBounds.y, CamBounds.m_minBounds.y, CamBounds.m_maxBounds.y);



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
AABB2 UIWidget::GetWidgetDimensions( const AABB2& parentBounds )
{
	float parentWidth = parentBounds.GetWidth();
	float parentHeight = parentBounds.GetHeight();

	//Get the size of the local box
	Vec2 BL = Vec2::ZERO;
	Vec2 TR = Vec2(parentWidth * m_virtualSize.x, parentHeight * m_virtualSize.y);

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
	if(m_parent != nullptr)				{	parentBounds = m_parent->GetWorldBounds();												   }
	else 								{	parentBounds = AABB2(Vec2::ZERO, Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT));}

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
