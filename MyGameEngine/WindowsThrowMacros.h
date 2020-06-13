#pragma once

#define WND_EXCEPT( hr ) Window::Exception( __LINE__,__FILE__,(hr) )
#define WND_LAST_EXCEPT() Window::Exception( __LINE__,__FILE__,GetLastError() )