/*
 * Copyright (C) 2016 Simon Fels <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ANBOX_PLATFORM_SDL_WINDOW_H_
#define ANBOX_PLATFORM_SDL_WINDOW_H_

#include "anbox/wm/window.h"
#include "anbox/platform/sdl/sdl_wrapper.h"

#include <EGL/egl.h>
#include<sys/time.h>

#include <atomic>
#include <memory>
#include <vector>
#include <map>

class Renderer;

namespace anbox {
namespace platform {
namespace sdl {
class Window : public std::enable_shared_from_this<Window>, public wm::Window {
 public:
  typedef std::int32_t Id;
  static Id Invalid;
  static const long long USEC_PER_SEC = 1000000;
  static const long long APP_START_MAX_TIME = 15 * USEC_PER_SEC;
  static const long long timespan_db_click = 500000;

  enum window_property{
    HIDE_BACK     = 0x01,
    HIDE_MINIMIZE = 0x02,
    HIDE_MAXIMIZE = 0x04,
    HIDE_CLOSE    = 0x08,
    SHOW_ALL      = 0x00
  };

  static const std::map<std::string, window_property> property_map;

  class Observer {
   public:
    virtual ~Observer();
    virtual void window_deleted(const Id &id) = 0;
    virtual void window_wants_focus(const Id &id) = 0;
    virtual void window_moved(const Id &id, const std::int32_t &x,
                              const std::int32_t &y) = 0;
    virtual void window_resized(const Id &id, const std::int32_t &x,
                                const std::int32_t &y) = 0;
    virtual void input_key_event(const SDL_Scancode &scan_code, std::int32_t down_or_up) = 0;
  };

  Window(const std::shared_ptr<Renderer> &renderer,
         const Id &id, const wm::Task::Id &task,
         const std::shared_ptr<Observer> &observer,
         const graphics::Rect &frame,
         const std::string &title,
         bool resizable);
  ~Window();

  bool title_event_filter(int y) override;
  void process_event(const SDL_Event &event);
  bool check_min_state() {return SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED;}
 
  void update_state(const wm::WindowState::List &states) override;

  bool check_db_clicked(int x, int y);
  EGLNativeWindowType native_handle() const override;
  Id id() const;
  std::uint32_t window_id() const;
  Uint32 GetWindowFlags(){return SDL_GetWindowFlags(window_);}
  inline window_property get_property() {
      return visible_property;
  }

 private:
  static SDL_HitTestResult on_window_hit(SDL_Window *window, const SDL_Point *pt, void *data);
  void close();
  void switch_window_state();

  std::atomic<bool> initialized{false};
  long long last_update_time{ 0 };
  Id id_;
  std::shared_ptr<Observer> observer_;
  EGLNativeDisplayType native_display_;
  EGLNativeWindowType native_window_;
  SDL_Window *window_;
  long long lastClickTime{ 0 };
  int last_point_x{ 0 };
  int last_point_y{ 0 };
  int last_wnd_x{ 0 };
  int last_wnd_y{ 0 };
  window_property visible_property{ SHOW_ALL };
};
} // namespace sdl
} // namespace platform
} // namespace anbox

#endif
