//------------------------------------------------------------------------------------------------------------------------------
#include "Game/UIWidget.hpp"
// Engine Systems
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/VertexUtils.hpp"

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
	m_worldBounds = container;
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::ProcessInput( InputEvent &evt )
{
	UNUSED(evt);
	
}

//------------------------------------------------------------------------------------------------------------------------------
void UIWidget::Render()
{
	//draw this widget in the screen using the position, anchor and pivot
	if(m_parent == nullptr)
	{
		g_renderContext->BindShader(g_renderContext->CreateOrGetShaderFromFile(m_defaultShaderName));
		g_renderContext->BindTextureViewWithSampler(0U, nullptr);
		g_renderContext->SetModelMatrix(Matrix44::IDENTITY);

		//We are the parent to all, draw the screen
		//IntVec2 screenDimensions = g_renderContext->GetCurrentScreenDimensions();
		
		AABB2 parentBounds = m_worldBounds;
		AABB2 BoxDimensions = GetWidgetDimensions(parentBounds);

		std::vector<Vertex_PCU> boxVerts;
		AABB2 boxDimensions(GetRelativePosInWorld2D(BoxDimensions.GetBottomLeft2D()), GetRelativePosInWorld2D(BoxDimensions.GetTopRight2D()));
		AddVertsForAABB2D(boxVerts, boxDimensions, Rgba::RED);

		g_renderContext->DrawVertexArray(boxVerts);
	}
	else
	{
		//Do nothing here
	}

	//Render my children here

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
	BL = center + Vec2(parentWidth * m_virtualSize.x, parentHeight * m_virtualSize.y)  * -1.f * m_pivot;
	TR = center + Vec2(parentWidth * m_virtualSize.x, parentHeight * m_virtualSize.y) * 1.f * m_pivot;



	AABB2 box = AABB2(BL, TR);

	return box;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 UIWidget::GetRelativePosInWorld2D( const Vec2& positionInWorld )
{
	Vec2 correctedPos;
	IntVec2 CamSize = g_renderContext->GetCurrentScreenDimensions();
	AABB2 CamBounds = AABB2(Vec2(CamSize.x * -0.5f, CamSize.y * -0.5f), Vec2(CamSize.x * 0.5f, CamSize.y * 0.5f));
	correctedPos.x = RangeMapFloat(positionInWorld.x, m_worldBounds.m_minBounds.x, m_worldBounds.m_maxBounds.x, CamBounds.m_minBounds.x, CamBounds.m_maxBounds.x);
	correctedPos.y = RangeMapFloat(positionInWorld.y, m_worldBounds.m_minBounds.y, m_worldBounds.m_maxBounds.y, CamBounds.m_minBounds.y, CamBounds.m_maxBounds.y);
	return correctedPos;
}

