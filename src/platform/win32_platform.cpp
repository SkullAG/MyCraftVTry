#include <windows.h>

#include "defines.h"
#include "platform.h"

#include <renderer/vk_renderer.cpp>

global_variable bool running = true;
global_variable HWND window;

LRESULT CALLBACK platform_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Esta funcion sera llamada cuando cualquier accion le ocurra a la ventana, como:
    // cerrarla a la fuerza, usar el boton de cerrar, escalar la ventana, mover el raton
    // Es una funcion de Eventos vamos.

    switch(msg){
        case WM_CLOSE:
        running = false;
        break;
    }

    return DefWindowProcA(window, msg, wParam, lParam);

}

bool platform_create_window()
{
    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASS wc = {};
    wc.lpfnWndProc = platform_window_callback;
    wc.hInstance = instance;
    wc.lpszClassName = "SKOVoxels" ;
    // Para modificar el cursor
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if(!RegisterClassA(&wc))
    {
        MessageBoxA(window, "Failed registering window class",  "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }
    // Aqui es donde se crea la ventana
    // Recomiendo mirar la guia de window para implementar mas cosas a la ventana
    window = CreateWindowExA(
        WS_EX_APPWINDOW,
        "SKOVoxels",
        "MyCraft",
        WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPED,
        100, 100, 1600, 720, 0 ,0, instance, 0);

    // Comprobando el numero de la ventana que tengo para compararla con el render del vulkan
    printf("[WINDOW] HWND: %p\n", window);
    printf("[WINDOW] IsWindow: %d\n", IsWindow(window));

    if(window == NULL || !IsWindow(window)){
        MessageBoxA(window, "Failed creating vindow", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }
    // Para hacer que aparezca, es bastante parecido al de OPENGL
    ShowWindow(window, SW_SHOW);

    return true;

}

void platform_update_window(HWND window)
{
    MSG msg;

    // En teoria es para hacer que la ventana deje de esperar para una respuesta
    // Con esto agarramos los mensajes que nos da y los borramos
    while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

int main()
{
    VkContext vkcontext = {};

    // En caso de que falle que no inicialice y se cierre
    if(!platform_create_window())
    {
        return -1;
    }
    if(!vk_init(&vkcontext, (void*)window))
    {
        return -1;
    }
    // Hace aparecer la ventana
    while(running)
    {
        platform_update_window(window);
        if(!vk_render(&vkcontext))
        {
            return -1;
        }
       
    }

    return 0;
}

void platform_get_window_size(uint32_t *width, uint32_t *height)
{   
    RECT rect;
    GetClientRect(window, &rect);

    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
}