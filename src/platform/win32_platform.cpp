#include <windows.h>

#include <renderer/vk_renderer.cpp>

static bool running = true;

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

bool platform_create_window(HWND *window)
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
        MessageBoxA(*window, "Failed registering window class",  "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }
    // Aqui es donde se crea la ventana
    // Recomiendo mirar la guia de window para implementar mas cosas a la ventana
    *window = CreateWindowExA(
        WS_EX_APPWINDOW,
        "SKOVoxels",
        "MyCraft",
        WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPED,
        100, 100, 1600, 720, 0 ,0, instance, 0);

    if(window == 0){
        MessageBoxA(*window, "Failed creating vindow", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }
    // Para hacer que aparezca, es bastante parecido al de OPENGL
    ShowWindow(*window, SW_SHOW);

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
    HWND window = 0;
    // En caso de que falle que no inicialice y se cierre
    if(!platform_create_window(&window))
    {
        return -1;
    }
     printf("Funciono");
    if(!vk_init(&vkcontext, &window))
    {
        return -1;
    }
    // Hace aparecer la ventana
    while(running)
    {
        platform_update_window(window);
       
    }

    return 0;
}