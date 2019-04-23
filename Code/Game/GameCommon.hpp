#pragma once
#include "Engine/Commons/EngineCommon.hpp"

constexpr float WORLD_WIDTH = 200.f;
constexpr float WORLD_HEIGHT = 100.f;
constexpr float WORLD_CENTER_X = WORLD_WIDTH / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_HEIGHT / 2.f;
constexpr float SCREEN_ASPECT = 16.f/9.f;

constexpr float CANVAS_WIDTH = 1280.f;
constexpr float CANVAS_HEIGHT = 720.f;
constexpr float CANVAS_ASPECT = 1.77f;

constexpr float CAMERA_SHAKE_REDUCTION_PER_SECOND = 1.f;
constexpr float MAX_SHAKE = 2.0f;

constexpr float DEVCONSOLE_LINE_HEIGHT = 2.0f;

constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area

constexpr float MAX_ZOOM_STEPS = 10.f;
constexpr float MIN_ZOOM_STEPS = -10.f;

constexpr float UI_SCREEN_ASPECT = 1.77f;
constexpr float UI_SCREEN_HEIGHT = 720.f;
constexpr float UI_NEGATIVE_ASPECT = 0.5625;

class RenderContext;
class InputSystem;
class AudioSystem;
class App;

extern RenderContext* g_renderContext;
extern InputSystem* g_inputSystem;
extern AudioSystem* g_audio;
extern App* g_theApp;