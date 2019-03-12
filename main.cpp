#include "gui\gui.h"
#include "gui\resource_managers.h"
#include "util\random.h"

int main() {
#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
	g_app = new gui::application(1280, 720);
	g_app->run();
	safe_delete(g_audio);
	safe_delete(g_fonts); 
	safe_delete(g_images);
	safe_delete(g_app);
	return 1;
}