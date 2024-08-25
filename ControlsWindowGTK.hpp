// =============================================================================
//  ControlsWindowGTK.hpp
//
//  MIT License
//
//  Copyright (c) 2022-2024 Dairoku Sekiguchi
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
// =============================================================================
/*!
  \file     ControlsWindowGTK.h
  \author   Dairoku Sekiguchi
  \version  3.0.0
  \date     2022/02/19
*/
#ifndef CONTROLS_WINDOW_GTK_H_
#define CONTROLS_WINDOW_GTK_H_
#define SHL_CONTROLS_WINDOW_GTK_BASE_VERSION   "3.0.0"

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <gtkmm.h>
#include <gtkmm/switch.h>

// Namespace -------------------------------------------------------------------
namespace shl::gtk
{
//
// =============================================================================
//  shl base gtk classes
// =============================================================================
// Macros ----------------------------------------------------------------------
#ifndef SHL_BASE_GTK_CLASS_
#define SHL_BASE_GTK_CLASS_
#define SHL_BASE_GTK_CLASS_VERSION  SHL_CONTROLS_WINDOW_GTK_BASE_VERSION
//
// Namespace -------------------------------------------------------------------
namespace base  // shl::gtk::base
{
// Log macros for debugging ----------------------------------------------------
#define SHL_LOG_STRINGIFY(x)    #x
#define SHL_LOG_TOSTRING(x)     SHL_LOG_STRINGIFY(x)
#define SHL_LOG_AT              SHL_LOG_TOSTRING(__FILE__) ":" SHL_LOG_TOSTRING(__LINE__)
#define SHL_LOG_LOCATION_MACRO  "(" SHL_LOG_AT ")"
#ifdef _MSC_VER
 #define __PRETTY_FUNCTION__  __FUNCTION__
#endif
#define SHL_FUNC_NAME_MACRO  __PRETTY_FUNCTION__
#if defined(SHL_LOG_LEVEL_ERR)
 #define SHL_LOG_LEVEL 1
#elif defined(SHL_LOG_LEVEL_WARN)
 #define SHL_LOG_LEVEL 2
#elif defined(SHL_LOG_LEVEL_INFO)
 #define SHL_LOG_LEVEL 3
#elif defined(SHL_LOG_LEVEL_DBG)
 #define SHL_LOG_LEVEL 4
#elif defined(SHL_LOG_LEVEL_TRACE)
 #define SHL_LOG_LEVEL 5
#endif
#ifdef SHL_LOG_LEVEL
 #define SHL_LOG_OUT(type, loc_str, func_str, out_str, ...) \
  printf(type " %s@" loc_str  " " out_str "\n", func_str , ##__VA_ARGS__)
 #if SHL_LOG_LEVEL > 0
  #define SHL_ERROR_OUT(out_str, ...) SHL_LOG_OUT("[ERROR]",\
       SHL_LOG_LOCATION_MACRO, SHL_FUNC_NAME_MACRO, out_str, ##__VA_ARGS__)
 #else
  #define SHL_ERROR_OUT(out_str, ...)
 #endif
 #if SHL_LOG_LEVEL > 1
  #define SHL_WARNING_OUT(out_str, ...) SHL_LOG_OUT("[WARN ]",\
       SHL_LOG_LOCATION_MACRO, SHL_FUNC_NAME_MACRO, out_str, ##__VA_ARGS__)
 #else
  #define SHL_WARNING_OUT(out_str, ...)
 #endif
 #if SHL_LOG_LEVEL > 2
  #define SHL_INFO_OUT(out_str, ...) SHL_LOG_OUT("[INFO ]",\
       SHL_LOG_LOCATION_MACRO, SHL_FUNC_NAME_MACRO, out_str, ##__VA_ARGS__)
 #else
  #define SHL_INFO_OUT(out_str, ...)
 #endif
 #if SHL_LOG_LEVEL > 3
  #define SHL_DBG_OUT(out_str, ...) SHL_LOG_OUT("[DEBUG]",\
   SHL_LOG_LOCATION_MACRO, SHL_FUNC_NAME_MACRO, out_str, ##__VA_ARGS__)
 #else
  #define SHL_DBG_OUT(out_str, ...)
 #endif
 #if SHL_LOG_LEVEL > 4
  #define SHL_TRACE_OUT(out_str, ...) SHL_LOG_OUT("[TRACE]",\
   SHL_LOG_LOCATION_MACRO, SHL_FUNC_NAME_MACRO, out_str, ##__VA_ARGS__)
 #else
  #define SHL_TRACE_OUT(out_str, ...)
 #endif
#else
 #define SHL_LOG_OUT(type, loc_str, func_str, out_str, ...)
 #define SHL_ERROR_OUT(out_str, ...)
 #define SHL_WARNING_OUT(out_str, ...)
 #define SHL_INFO_OUT(out_str, ...)
 #define SHL_DBG_OUT(out_str, ...)
 #define SHL_TRACE_OUT(out_str, ...)
#endif

  // ===========================================================================
  //  BackgroundAppWindowInterface class
  // ===========================================================================
  class BackgroundAppWindowInterface
  {
  public:
    virtual Gtk::Window *back_app_create_window(const char *in_title) = 0;
    virtual void back_app_wait_new_window() = 0;
    virtual Gtk::Window *back_app_get_window() = 0;
    virtual bool back_app_delete_request() = 0;
    virtual void back_app_delete_window() = 0;
    virtual bool back_app_is_window_deleted() = 0;
    virtual void back_app_wait_delete_window() = 0;
    virtual void back_app_update_window() = 0;
  };

  // ===========================================================================
  //  EventData class
  // ===========================================================================
  class EventData
  {
  public:
    // -------------------------------------------------------------------------
    // EventData destructor
    // -------------------------------------------------------------------------
    virtual ~EventData() = default;
    // -------------------------------------------------------------------------
    // get_source
    // -------------------------------------------------------------------------
    virtual void *get_source()
    {
      return m_source;
    }

  protected:
    // -------------------------------------------------------------------------
    // EventData constructor
    // -------------------------------------------------------------------------
    EventData(void *in_source, void (*in_handler)(EventData *)) :
        m_source(in_source),
        m_handler(in_handler)
    {
    }
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // is_same_source
    // -------------------------------------------------------------------------
    virtual bool is_same_source(EventData *in_event_data)
    {
      if (m_source == in_event_data->m_source &&
          m_handler       == in_event_data->m_handler)
        return true;
      return false;
    }
    // -------------------------------------------------------------------------
    // invoke_handler
    // -------------------------------------------------------------------------
    virtual void invoke_handler()
    {
      if (m_handler != nullptr)
        m_handler(this);
    }

  private:
    // member variables --------------------------------------------------------
    void  *m_source;
    void (*m_handler)(EventData *);

    // friend classes ----------------------------------------------------------
    friend class EventQueue;
    friend class WindowBase;
  };

  // ===========================================================================
  //  EventQueue class
  // ===========================================================================
  class EventQueue
  {
  public:
    // -------------------------------------------------------------------------
    // EventQueue constructor
    // -------------------------------------------------------------------------
    EventQueue() = default;
    // -------------------------------------------------------------------------
    // EventQueue destructor
    // -------------------------------------------------------------------------
    virtual ~EventQueue() = default;
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // push
    // -------------------------------------------------------------------------
    void push(void *in_source, void (*in_handler)(EventData *))
    {
      auto *event = new EventData(in_source, in_handler);
      push(event);
    }
    // -------------------------------------------------------------------------
    // push
    // -------------------------------------------------------------------------
    void push(EventData *in_event)
    {
      std::lock_guard<std::mutex> lock(m_event_queue_mutex);
      m_event_data_queue.push_back(in_event);
      m_new_event_cond.notify_all();
    }
    // -------------------------------------------------------------------------
    // notify
    // -------------------------------------------------------------------------
    void notify()
    {
      std::lock_guard<std::mutex> lock(m_event_queue_mutex);
      m_new_event_cond.notify_all();
    }
    // -------------------------------------------------------------------------
    // wait
    // -------------------------------------------------------------------------
    void wait()
    {
      std::unique_lock<std::mutex> lock(m_event_queue_mutex);
      if (m_event_data_queue.empty() == false)
        return;
      m_new_event_cond.wait(lock);
    }
    // -------------------------------------------------------------------------
    // process_events
    // -------------------------------------------------------------------------
    void process_events(bool in_last_only = false)
    {
      std::lock_guard<std::mutex> lock(m_event_queue_mutex);
      while (m_event_data_queue.empty() == false)
      {
        bool skip = false;
        EventData *event_data = m_event_data_queue.front();
        if (in_last_only)
        {
          auto it = m_event_data_queue.begin();
          it++;
          while (it != m_event_data_queue.end())
          {
            if (event_data->is_same_source(*it))
            {
              skip = true;
              break;
            }
            it++;
          }
        }
        if (skip == false)
          event_data->invoke_handler();
        m_event_data_queue.pop_front();
        delete event_data;
      }
    }

  private:
    // member variables --------------------------------------------------------
    std::list<EventData *>  m_event_data_queue;
    std::condition_variable m_new_event_cond;
    std::mutex  m_event_queue_mutex;
  };

  // ===========================================================================
  //  TimerData class
  // ===========================================================================
  class TimerData
  {
  protected:
    // -------------------------------------------------------------------------
    // TimerData constructor
    // -------------------------------------------------------------------------
    TimerData( unsigned int in_interval_ms,
                EventQueue *in_user_event_queue,
                void (*in_timer_event_func)(void *in_user_data),
                void *in_user_data = nullptr) :
        m_interval_ms(in_interval_ms),
        m_user_event_queue(in_user_event_queue),
        m_timer_event_func(in_timer_event_func),
        m_user_data(in_user_data),
        m_is_running(false)
    {
    }
    // -------------------------------------------------------------------------
    // WidgetData destructor
    // -------------------------------------------------------------------------
    virtual ~TimerData() = default;
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // connect (needs to be called from the UI thread)
    // -------------------------------------------------------------------------
    void connect()
    {
      mTimerConnection = Glib::signal_timeout().connect(
              sigc::mem_fun(*this, &TimerData::queue_timer_event), m_interval_ms);
      m_is_running = true;
      // todo : add connect_seconds() path also
    }
    // -------------------------------------------------------------------------
    // disconnect (needs to be called from the UI thread)
    // -------------------------------------------------------------------------
    void disconnect()
    {
      if (m_is_running == false)
        return;
      mTimerConnection.disconnect();
      m_is_running = false;
    }
    // -------------------------------------------------------------------------
    // queue_timer_event
    // -------------------------------------------------------------------------
    bool queue_timer_event()
    {
      if (m_user_event_queue == nullptr || m_timer_event_func == nullptr)
        return false; // return false = disconnect

      m_user_event_queue->push(this, process_timer_event);
      return true;  
    }
    // -------------------------------------------------------------------------
    // process_timer_event
    // -------------------------------------------------------------------------
    static void process_timer_event(base::EventData *in_event)
    {
      auto  *timer = (TimerData *)in_event->get_source();
      timer->m_timer_event_func(timer->m_user_data);
    }

  private:
    // member variables --------------------------------------------------------
    unsigned int  m_interval_ms;
    void (*m_timer_event_func)(void *in_user_data);
    void *m_user_data;
    EventQueue *m_user_event_queue;
    sigc::connection  mTimerConnection;
    bool  m_is_running;

    friend class BackgroundApp;
    friend class WindowBase;
  };

  // ===========================================================================
  //  BackgroundApp class
  // ===========================================================================
  class BackgroundApp : public Gtk::Application
  {
  public:
    // -------------------------------------------------------------------------
    // BackgroundApp destructor
    // -------------------------------------------------------------------------
    ~BackgroundApp() override
    {
      SHL_DBG_OUT("BackgroundApp was deleted");
    }

  protected:
    // -------------------------------------------------------------------------
    // BackgroundApp constructor
    // -------------------------------------------------------------------------
    BackgroundApp() :
            Gtk::Application("org.gtkmm.examples.application",
                             Gio::APPLICATION_NON_UNIQUE),
                             m_quit(false)
    {
    }

    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // post_create_window
    // -------------------------------------------------------------------------
    // [Note] this function will be called from another thread
    //
    void post_create_window(BackgroundAppWindowInterface *in_interface, const char *in_title)
    {
      std::lock_guard<std::mutex> lock(m_create_win_queue_mutex);
      m_create_win_queue.push(in_interface);
      m_win_title_queue.push(in_title);
      // Invoke one time on_idle call (by returning false from the signal handler)
      Glib::signal_idle().connect(sigc::mem_fun(*this, &BackgroundApp::on_idle));
    }
    // -------------------------------------------------------------------------
    // post_delete_window
    // -------------------------------------------------------------------------
    // [Note] this function will be called from another thread
    //
    void post_delete_window(BackgroundAppWindowInterface *in_interface)
    {
      std::lock_guard<std::mutex> lock(m_delete_win_queue_mutex);
      m_delete_win_queue.push(in_interface);
      // Invoke one time on_idle call (by returning false from the signal handler)
      Glib::signal_idle().connect(sigc::mem_fun(*this, &BackgroundApp::on_idle));
    }
    // -------------------------------------------------------------------------
    // post_update_window
    // -------------------------------------------------------------------------
    // [Note] this function will be called from another thread
    //
    void post_update_window(BackgroundAppWindowInterface *in_interface)
    {
      std::lock_guard<std::mutex> lock(m_update_win_queue_mutex);
      m_update_win_queue.push(in_interface);
      // Invoke one time on_idle call (by returning false from the signal handler)
      Glib::signal_idle().connect(sigc::mem_fun(*this, &BackgroundApp::on_idle));
    }
    // -------------------------------------------------------------------------
    // post_connect_timer
    // -------------------------------------------------------------------------
    // [Note] this function will be called from another thread
    //
    void post_connect_timer(TimerData *inTimerData)
    {
      std::lock_guard<std::mutex> lock(m_connect_timer_queue_mutex);
      m_connect_timer_queue.push(inTimerData);
      // Invoke one time on_idle call (by returning false from the signal handler)
      Glib::signal_idle().connect(sigc::mem_fun(*this, &BackgroundApp::on_idle));
    }
    // -------------------------------------------------------------------------
    // post_disconnect_timer
    // -------------------------------------------------------------------------
    // [Note] this function will be called from another thread
    //
    void post_disconnect_timer(TimerData *inTimerData)
    {
      std::lock_guard<std::mutex> lock(m_disconnect_timer_queue_mutex);
      m_disconnect_timer_queue.push(inTimerData);
      // Invoke one time on_idle call (by returning false from the signal handler)
      Glib::signal_idle().connect(sigc::mem_fun(*this, &BackgroundApp::on_idle));
    }
    // -------------------------------------------------------------------------
    // post_quit_app
    // -------------------------------------------------------------------------
    // [Note] this function will be called from another thread
    //
    void post_quit_app()
    {
      std::lock_guard<std::mutex> lock(m_create_win_queue_mutex);
      m_quit = true;
      // Invoke one time on_idle call (by returning false from the signal handler)
      Glib::signal_idle().connect(sigc::mem_fun(*this, &BackgroundApp::on_idle));
    }
    // -------------------------------------------------------------------------
    // get_window_num
    // -------------------------------------------------------------------------
    size_t get_window_num()
    {
      return m_window_list.size();
    }
    // -------------------------------------------------------------------------
    // wait_window_all_closed
    // -------------------------------------------------------------------------
    void wait_window_all_closed()
    {
      std::unique_lock<std::mutex> window_lock(m_window_mutex);
      if (get_window_num() == 0)
        return;
      m_window_cond.wait(window_lock);
    }
    // -------------------------------------------------------------------------
    // on_activate
    // -------------------------------------------------------------------------
    void on_activate() override
    {
      // The application has been started, so let's show a window.
      process_create_windows();
    }
    // -------------------------------------------------------------------------
    // on_delete_event
    // -------------------------------------------------------------------------
    bool on_delete_event(GdkEventAny *in_event)
    {
      for (auto it = m_window_list.begin(); it != m_window_list.end(); it++)
      {
        //if (in_window == (*it)->back_app_get_window())
        if (in_event->window == (*it)->back_app_get_window()->get_window()->gobj())
          return (*it)->back_app_delete_request();
      }
      return false;
    }
    // -------------------------------------------------------------------------
    // on_hide_window
    // -------------------------------------------------------------------------
    void on_hide_window(Gtk::Window *in_window)
    {
      for (auto it = m_window_list.begin(); it != m_window_list.end(); it++)
      {
        if (in_window == (*it)->back_app_get_window())
        {
          (*it)->back_app_delete_window();
          m_window_list.erase(it);
          break;
        }
      }
      if (m_window_list.empty())
        m_window_cond.notify_all();
    }
    // -------------------------------------------------------------------------
    // on_idle
    // -------------------------------------------------------------------------
    bool on_idle()
    {
      SHL_DBG_OUT("on_idle() was called");
      process_create_windows();
      process_update_windows();
      process_delete_windows();
      process_connect_timers();
      process_disconnect_timers();
      if (m_quit)
        quit();
      // by returning false here, this signal handler will be disconnected
      // from Glib::signal_idle()
      // https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-idle-functions.html
      return false;
    }
    // -------------------------------------------------------------------------
    // process_create_windows
    // -------------------------------------------------------------------------
    void process_create_windows()
    {
      std::lock_guard<std::mutex> lock(m_create_win_queue_mutex);
      while (!m_create_win_queue.empty())
      {
        BackgroundAppWindowInterface *interface = m_create_win_queue.front();
        const char *title = m_win_title_queue.front();
        auto it = std::find(m_window_list.begin(), m_window_list.end(), interface);
        if (it == m_window_list.end())
        {
          Gtk::Window *win = interface->back_app_create_window(title);
          add_window(*win);
          win->signal_delete_event().connect(sigc::mem_fun(*this,
                            &BackgroundApp::on_delete_event));
          win->signal_hide().connect(sigc::bind<Gtk::Window *>(
                  sigc::mem_fun(*this,
                                &BackgroundApp::on_hide_window), win));
          win->present();
          m_window_list.push_back(interface);
        }
        m_create_win_queue.pop();
        m_win_title_queue.pop();
      }
    }
    // -------------------------------------------------------------------------
    // process_delete_windows
    // -------------------------------------------------------------------------
    void process_delete_windows()
    {
      std::lock_guard<std::mutex> lock(m_delete_win_queue_mutex);
      while (!m_delete_win_queue.empty())
      {
        BackgroundAppWindowInterface *interface = m_delete_win_queue.front();
        auto it = std::find(m_window_list.begin(), m_window_list.end(), interface);
        if (it != m_window_list.end())
        {
          Gtk::Window *win = interface->back_app_get_window();
          win->close();
          remove_window(*win);
          interface->back_app_delete_window();
          m_window_list.erase(it);
        }
        m_delete_win_queue.pop();
      }
    }
    // -------------------------------------------------------------------------
    // process_update_windows
    // -------------------------------------------------------------------------
    void process_update_windows()
    {
      std::lock_guard<std::mutex> lock(m_update_win_queue_mutex);
      while (!m_update_win_queue.empty())
      {
        BackgroundAppWindowInterface *interface = m_update_win_queue.front();
        auto it = std::find(m_window_list.begin(), m_window_list.end(), interface);
        if (it != m_window_list.end())
          interface->back_app_update_window();
        m_update_win_queue.pop();
      }
    }
    // -------------------------------------------------------------------------
    // process_connect_timers
    // -------------------------------------------------------------------------
    void process_connect_timers()
    {
      std::lock_guard<std::mutex> lock(m_connect_timer_queue_mutex);
      while (!m_connect_timer_queue.empty())
      {
        TimerData *timer = m_connect_timer_queue.front();
        timer->connect();
        m_connect_timer_queue.pop();
      }
    }
    // -------------------------------------------------------------------------
    // process_disconnect_timers
    // -------------------------------------------------------------------------
    void process_disconnect_timers()
    {
      std::lock_guard<std::mutex> lock(m_disconnect_timer_queue_mutex);
      while (!m_disconnect_timer_queue.empty())
      {
        TimerData *timer = m_disconnect_timer_queue.front();
        timer->disconnect();
        m_disconnect_timer_queue.pop();
      }
    }

  private:
    // member variables --------------------------------------------------------
    std::mutex m_create_win_queue_mutex;
    std::queue<BackgroundAppWindowInterface *> m_create_win_queue;
    std::queue<const char *> m_win_title_queue;
    std::mutex m_delete_win_queue_mutex;
    std::queue<BackgroundAppWindowInterface *> m_delete_win_queue;
    std::mutex m_update_win_queue_mutex;
    std::queue<BackgroundAppWindowInterface *> m_update_win_queue;
    //
    std::mutex m_connect_timer_queue_mutex;
    std::queue<TimerData *> m_connect_timer_queue;
    std::mutex m_disconnect_timer_queue_mutex;
    std::queue<TimerData *> m_disconnect_timer_queue;
    //
    std::vector<BackgroundAppWindowInterface *> m_window_list;
    std::condition_variable m_window_cond;
    std::mutex  m_window_mutex;
    bool m_quit;

    // friend classes ----------------------------------------------------------
    friend class BackgroundAppRunner;
  };

  // ===========================================================================
  //  BackgroundAppRunner class
  // ===========================================================================
  class BackgroundAppRunner
  {
  public:
    // -------------------------------------------------------------------------
    // BackgroundAppRunner destructor
    // -------------------------------------------------------------------------
    virtual ~BackgroundAppRunner()
    {
      if (m_app != nullptr)
        m_app->post_quit_app();
      if (m_thread != nullptr)
      {
        m_thread->join();
        delete m_thread;
        delete m_app;
      }
      SHL_DBG_OUT("BackgroundAppRunner was deleted");
    }

  protected:
    // -------------------------------------------------------------------------
    // BackgroundAppRunner constructor
    // -------------------------------------------------------------------------
    BackgroundAppRunner() :
      m_app(nullptr), m_thread(nullptr)
    {
    }

    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // wait_window_all_closed
    // -------------------------------------------------------------------------
    void wait_window_all_closed()
    {
      std::lock_guard<std::mutex> lock(m_function_call_mutex);
      if (m_app == nullptr)
        return;
      m_app->wait_window_all_closed();
    }
    // -------------------------------------------------------------------------
    // get_window_num
    // -------------------------------------------------------------------------
    size_t get_window_num()
    {
      std::lock_guard<std::mutex> lock(m_function_call_mutex);
      if (m_app == nullptr)
        return 0;
      return m_app->get_window_num();
    }
    // -------------------------------------------------------------------------
    // is_window_close_all
    // -------------------------------------------------------------------------
    bool is_window_close_all()
    {
      if (get_window_num() == 0)
        return true;
      return false;
    }
    // -------------------------------------------------------------------------
    // create_window
    // -------------------------------------------------------------------------
    void create_window(BackgroundAppWindowInterface *in_interface, const char *in_title)
    {
      std::lock_guard<std::mutex> lock(m_function_call_mutex);
      if (m_app == nullptr)
      {
        m_app = new BackgroundApp();
        m_app->hold();
      }
      m_app->post_create_window(in_interface, in_title);
      if (m_thread != nullptr)
        return;
      m_thread = new std::thread(thread_func, this);
    }
    // -------------------------------------------------------------------------
    // delete_window
    // -------------------------------------------------------------------------
    void delete_window(BackgroundAppWindowInterface *in_interface)
    {
      std::lock_guard<std::mutex> lock(m_function_call_mutex);
      if (m_app == nullptr)
        return;
      m_app->post_delete_window(in_interface);
    }
    // -------------------------------------------------------------------------
    // update_window
    // -------------------------------------------------------------------------
    void update_window(BackgroundAppWindowInterface *in_interface)
    {
      std::lock_guard<std::mutex> lock(m_function_call_mutex);
      if (m_app == nullptr)
        return;
      m_app->post_update_window(in_interface);
    }
    // -------------------------------------------------------------------------
    // connect_timer
    // -------------------------------------------------------------------------
    void connect_timer(TimerData *inTimerData)
    {
      std::lock_guard<std::mutex> lock(m_function_call_mutex);
      if (m_app == nullptr)
        return;
      m_app->post_connect_timer(inTimerData);
    }
    // -------------------------------------------------------------------------
    // disconnect_timer
    // -------------------------------------------------------------------------
    void disconnect_timer(TimerData *inTimerData)
    {
      std::lock_guard<std::mutex> lock(m_function_call_mutex);
      if (m_app == nullptr)
        return;
      m_app->post_disconnect_timer(inTimerData);
    }

    // static functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // get_runner
    // -------------------------------------------------------------------------
    static BackgroundAppRunner *get_runner()
    {
      static BackgroundAppRunner s_runner;
      return &s_runner;
    }

  private:
    // member variables --------------------------------------------------------
    BackgroundApp *m_app;
    std::thread *m_thread;
    std::mutex m_function_call_mutex;

    // static functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // thread_func
    // -------------------------------------------------------------------------
    static void thread_func(BackgroundAppRunner *in_obj)
    {
      SHL_TRACE_OUT("thread started");
      in_obj->m_app->run();
      SHL_TRACE_OUT("thread ended");
    }

    // friend classes ----------------------------------------------------------
    friend class WindowBase;
  };

  // ===========================================================================
  //  WindowBase class
  // ===========================================================================
  class WindowBase : private BackgroundAppWindowInterface
  {
  public:
    // -------------------------------------------------------------------------
    // WindowBase destructor
    // -------------------------------------------------------------------------
    virtual ~WindowBase()
    {
      kill_timer(nullptr);  // stop and delete all timers (just in case)
      m_app_runner->delete_window(this);
    }

    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // wait_window_closed
    // -------------------------------------------------------------------------
    /**
     * Waits until the window associated to the object is closed.
     * @note This is a blocking function.
     *
     */
    void wait_window_closed()
    {
      back_app_wait_delete_window();
    }
    // -------------------------------------------------------------------------
    // is_window_closed
    // -------------------------------------------------------------------------
    /**
     * Checks whether the opened window is closed or not.
     * @ The window associated to this object
     *
     * @return Whether the window is closed or not
     *  - true : The window was closed
     *  - false : The window is not closed
     */
    bool is_window_closed()
    {
      return back_app_is_window_deleted();
    }
    // -------------------------------------------------------------------------
    // wait_window_close_all
    // -------------------------------------------------------------------------
    /**
     * Waits until all of the windows are closed.
     * @note The windows including opened by the other objects
     */
    void wait_window_close_all()
    {
      m_app_runner->wait_window_all_closed();
    }
    // -------------------------------------------------------------------------
    // is_window_close_all
    // -------------------------------------------------------------------------
    /**
     * Checks whether the all windows are closed or not.
     * @note The windows including opened by the other objects
     *
     * @return Whether the window is closed or not
     *  - true : The windows were all closed
     *  - false : The all of the windows are still not closed
     */
    bool is_window_close_all()
    {
      return m_app_runner->is_window_close_all();
    }
    // -------------------------------------------------------------------------
    // get_window_num
    // -------------------------------------------------------------------------
    /**
     * Retrieves the number of opened windows.
     * @note The windows including opened by the other objects
     *
     * @return the number of opened windows
     */
    size_t get_window_num()
    {
      return m_app_runner->get_window_num();
    }
    // -------------------------------------------------------------------------
    // show_window
    // -------------------------------------------------------------------------
    /**
     * Shows a window.
     * @note if in_title is nullptr, the title will be automatically generated.
     *
     * @param in_title  The title of the window
     */
    void show_window(const char *in_title = nullptr)
    {
      if (back_app_get_window() != nullptr)
        return;
      m_app_runner->create_window(this, in_title);
      back_app_wait_new_window();
      start_all_timers();
    }
    // -------------------------------------------------------------------------
    // update
    // -------------------------------------------------------------------------
    /**
     * Tells the library to updates the window.
     * @note This function needs to be called when the content of the object is
     * updated
     */
    void update()
    {
      if (back_app_get_window() == nullptr)
        return;
      m_app_runner->update_window(this);
    }
    // -------------------------------------------------------------------------
    // get_user_event_queue()
    // -------------------------------------------------------------------------
    EventQueue *get_user_event_queue()
    {
      if (m_user_event_queue != nullptr)
        return m_user_event_queue;
      return get_user_global_queue();
    }
    // -------------------------------------------------------------------------
    // push_update_event()
    // -------------------------------------------------------------------------
    void push_update_event(void *inSource, void (*in_func)(EventData *))
    {
      m_background_queue.push(inSource, in_func);
    }
    // -------------------------------------------------------------------------
    // push_update_event()
    // -------------------------------------------------------------------------
    void push_update_event(EventData *in_update)
    {
      m_background_queue.push(in_update);
    }
    // -------------------------------------------------------------------------
    // add_close_event_listener
    // -------------------------------------------------------------------------
    void add_close_event_listener(base::EventQueue *in_event_queue)
    {
      m_close_notify_list.push_back(in_event_queue);
    }
    // -------------------------------------------------------------------------
    // connect_timer
    // -------------------------------------------------------------------------
    TimerData *add_timer(
                  unsigned int in_interval_ms,
                  void (*in_timer_event_func)(void *in_user_data) = nullptr,
                  void *in_user_data = nullptr,
                  base::EventQueue *in_user_event_queue = nullptr)
    {
      if (in_user_event_queue == nullptr)
        in_user_event_queue = get_user_event_queue();
      TimerData *timer;
      timer = new TimerData(
              in_interval_ms,
              in_user_event_queue,
              in_timer_event_func,
              in_user_data);
      m_timer_list.push_back(timer);
      return timer;
    }
    // -------------------------------------------------------------------------
    // kill_timer
    // -------------------------------------------------------------------------
    // [note]
    // after calling this function, passed inTimeData will be no
    // longer valid (the linked object will be deleted internally)
    // specifying nullptr will remove all timers associated with this window
    //
    void kill_timer(TimerData *inTimerData = nullptr)
    {
      if (inTimerData == nullptr)
        return;
      
      for (auto it = m_timer_list.begin(); it != m_timer_list.end(); it++)
        if ((*it) == inTimerData || inTimerData == nullptr)
        {
          m_app_runner->disconnect_timer((*it));
          delete (*it);
          if (inTimerData != nullptr)
          {
            m_timer_list.erase(it);
            break;
          }
        }
      if (inTimerData == nullptr)
        m_timer_list.clear();
    }

  protected:
    // Virtual Member functions --------------------------------------------------------
    virtual Gtk::Window *create_window_object(const char *in_title) = 0;
    virtual Gtk::Window *get_window_object() = 0;
    virtual void delete_window_object() = 0;
    virtual bool is_window_object_null() = 0;
    virtual void update_window() = 0;
    virtual const char *get_default_window_title() = 0;

    // -------------------------------------------------------------------------
    // WindowBase constructor
    // -------------------------------------------------------------------------
    WindowBase(EventQueue *in_user_event_queue = nullptr) :
      m_user_event_queue(in_user_event_queue)
    {
      m_app_runner = BackgroundAppRunner::get_runner();
    }
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // process_update_events()
    // -------------------------------------------------------------------------
    void process_update_events(bool in_last_only = false)
    {
      m_background_queue.process_events(in_last_only);
    }
    // -------------------------------------------------------------------------
    // start_all_timers
    // -------------------------------------------------------------------------
    void start_all_timers()
    {
      for (auto it = m_timer_list.begin(); it != m_timer_list.end(); it++)
        m_app_runner->connect_timer((*it));
    }

  private:
    // member variables --------------------------------------------------------
    BackgroundAppRunner *m_app_runner;
    EventQueue  m_background_queue;
    EventQueue  *m_user_event_queue;
    std::condition_variable m_new_window_cond;
    std::mutex  m_new_window_mutex;
    std::condition_variable m_delete_window_cond;
    std::mutex  m_delete_window_mutex;
    std::vector<base::EventQueue *> m_close_notify_list;
    std::vector<TimerData *>   m_timer_list;

    // BackgroundAppWindowInterface functions ----------------------------------
    // -------------------------------------------------------------------------
    // back_app_create_window (called from the UI thread)
    // -------------------------------------------------------------------------
    Gtk::Window *back_app_create_window(const char *in_title) override
    {
      static int window_num = 0;
      char buf[256];
      if (in_title != nullptr)
        if (*in_title == 0)
          in_title = nullptr;
      if (in_title == nullptr)
      {
        const char *title = get_default_window_title();
        if (window_num == 0)
          sprintf(buf, "%s", title);
        else
          sprintf(buf, "%s_%d", title, window_num);
        in_title = buf;
      }
      Gtk::Window *window = create_window_object(in_title);
      m_new_window_cond.notify_all();
      window_num++;
      return window;
    }
    // -------------------------------------------------------------------------
    // back_app_wait_new_window (called from the UI thread)
    // -------------------------------------------------------------------------
    void back_app_wait_new_window() override
    {
      std::unique_lock<std::mutex> lock(m_new_window_mutex);
      m_new_window_cond.wait(lock);
    }
    // -------------------------------------------------------------------------
    // back_app_delete_request (called from the UI thread)
    // -------------------------------------------------------------------------
    Gtk::Window *back_app_get_window() override
    {
      return get_window_object();
    }
    // -------------------------------------------------------------------------
    // back_app_delete_request (called from the UI thread)
    // -------------------------------------------------------------------------
    bool back_app_delete_request() override
    {
      return false; // by returning true, we can cancel window close
    }
    // -------------------------------------------------------------------------
    // back_app_delete_window (called from the UI thread)
    // -------------------------------------------------------------------------
    void back_app_delete_window() override
    {
      // we need to disconnect all timers first
      for (auto it = m_timer_list.begin(); it != m_timer_list.end(); it++)
        (*it)->disconnect();
      delete_window_object();
      m_delete_window_cond.notify_all();
      // We need to notify all event queues to un-block event queue's wait()
      for (auto it = m_close_notify_list.begin(); it != m_close_notify_list.end(); it++)
        (*it)->notify();
    }
    // -------------------------------------------------------------------------
    // back_app_is_window_deleted (called from the UI thread)
    // -------------------------------------------------------------------------
    bool back_app_is_window_deleted() override
    {
      return is_window_object_null();
    }
    // -------------------------------------------------------------------------
    // back_app_wait_delete_window (called from the UI thread)
    // -------------------------------------------------------------------------
    void back_app_wait_delete_window() override
    {
      std::unique_lock<std::mutex> lock(m_delete_window_mutex);
      m_delete_window_cond.wait(lock);
    }
    // -------------------------------------------------------------------------
    // back_app_update_window
    // -------------------------------------------------------------------------
    void back_app_update_window() override
    {
      update_window();
    }
    // static functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // get_user_global_queue
    // -------------------------------------------------------------------------
    static EventQueue *get_user_global_queue()
    {
      static EventQueue s_user_global_queue;
      return &s_user_global_queue;
    }
  };
} // namespace shl::gtk::base
#else
  // If another shl file is already included, we need to is_valid the base gtk class version
  #if SHL_BASE_GTK_CLASS_VERSION != SHL_CONTROLS_WINDOW_GTK_BASE_VERSION
    #error invalid shl base class version (There is a version inconsistency between included shl files)
  #endif
  //
#endif  // SHL_BASE_GTK_CLASS_

// The followings are for the friend class definition
class ControlsWindow;

// Namespace -------------------------------------------------------------------
namespace controls     // shl::gtk::controls
{
  // ===========================================================================
  //  WidgetData class
  // ===========================================================================
  class WidgetData
  {
  public:
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // is_updated
    // -------------------------------------------------------------------------
    bool is_updated() const
    {
      return m_is_updated;
    }
    // -------------------------------------------------------------------------
    // get_user_event_queue()
    // -------------------------------------------------------------------------
    base::EventQueue *get_user_event_queue()
    {
      if (m_user_event_queue != nullptr)
        return m_user_event_queue;
      return m_window->get_user_event_queue();
    }

  protected:
    // -------------------------------------------------------------------------
    // WidgetData constructor
    // -------------------------------------------------------------------------
    WidgetData(base::WindowBase *in_window,
            const char *in_label_str,
            base::EventQueue *in_user_event_queue = nullptr) :
        m_window(in_window),
        m_label_str(in_label_str),
        m_user_event_queue(in_user_event_queue),
        m_horiz_box(nullptr), m_label(nullptr),
        m_is_updated(false)
    {
    }
    // -------------------------------------------------------------------------
    // WidgetData destructor
    // -------------------------------------------------------------------------
    virtual ~WidgetData()
    {
      delete m_horiz_box;
      delete m_label;
    }
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // create
    // -------------------------------------------------------------------------
    virtual Gtk::Box *create()
    {
      m_horiz_box = new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL);
      m_label = new Gtk::Label(m_label_str.c_str());

      m_horiz_box->set_margin_left(8);
      m_horiz_box->set_margin_right(8);
      m_horiz_box->set_margin_top(2);
      m_horiz_box->set_margin_bottom(2);
      m_horiz_box->pack_start(*m_label, Gtk::PACK_SHRINK);
      return m_horiz_box;
    }
    // -------------------------------------------------------------------------
    // mark_as_updated()
    // -------------------------------------------------------------------------
    void mark_as_updated()
    {
      m_is_updated = true;
    }
    // -------------------------------------------------------------------------
    // push_event()
    // -------------------------------------------------------------------------
    void push_event(void (*in_func)(base::EventData *))
    {
      get_user_event_queue()->push(this, in_func);
    }
    // -------------------------------------------------------------------------
    // push_event()
    // -------------------------------------------------------------------------
    void push_event(base::EventData *in_event)
    {
      get_user_event_queue()->push(in_event);
    }
    // -------------------------------------------------------------------------
    // push_update()
    // -------------------------------------------------------------------------
    void push_update(void (*in_func)(base::EventData *))
    {
      m_window->push_update_event(this, in_func);
    }
    // -------------------------------------------------------------------------
    // push_update()
    // -------------------------------------------------------------------------
    void push_update(base::EventData *in_update)
    {
      m_window->push_update_event(in_update);
    }
    // -------------------------------------------------------------------------
    // invoke_update()
    // -------------------------------------------------------------------------
    void invoke_update()
    {
      m_window->update();
    }

  private:
    // member variables --------------------------------------------------------
    base::WindowBase *m_window;
    base::EventQueue *m_user_event_queue;

    Gtk::Box    *m_horiz_box;
    Gtk::Label  *m_label;
    std::string  m_label_str;
    bool  m_is_updated;

    friend class WindowData;
    friend class WindowView;
  };

  // ===========================================================================
  //  ButtonData class (GtkButton)
  // ===========================================================================
  class ButtonData : public WidgetData
  {
  protected:
    // -------------------------------------------------------------------------
    // ButtonData destructor
    // -------------------------------------------------------------------------
    ~ButtonData() override
    {
      delete m_button;
    }

    // -------------------------------------------------------------------------
    // ButtonData constructor
    // -------------------------------------------------------------------------
    ButtonData(base::WindowBase *in_window,
               const char *in_label_str,
               const char *in_button_str,
               void *in_user_data = nullptr,
               void (*in_clicked_func)(void *in_user_data) = nullptr,
               void (*in_pressed_func)(void *in_user_data) = nullptr,
               void (*in_released_func)(void *in_user_data) = nullptr,
               base::EventQueue *in_user_event_queue = nullptr) :
            WidgetData(in_window, in_label_str, in_user_event_queue),
            m_button_str(in_button_str),
            m_user_data(in_user_data),
            m_clicked_func(in_clicked_func),
            m_pressed_func(in_pressed_func),
            m_released_func(in_released_func),
            m_button(nullptr)
    {
    }
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // create
    // -------------------------------------------------------------------------
    Gtk::Box *create() override
    {
      Gtk::Box  *box = WidgetData::create();
      if (box == nullptr)
        return nullptr;
      m_button = new Gtk::Button(m_button_str.c_str());
      if (m_button == nullptr)
        return nullptr;
      m_button->signal_clicked().connect(
              sigc::mem_fun(*this, &ButtonData::queue_button_clicked));
      m_button->signal_pressed().connect(
              sigc::mem_fun(*this, &ButtonData::queue_button_pressed));
      m_button->signal_released().connect(
              sigc::mem_fun(*this, &ButtonData::queue_button_released));
      box->pack_end(*m_button, Gtk::PACK_SHRINK);
      return box;
    }
    // -------------------------------------------------------------------------
    // queue_button_clicked
    // -------------------------------------------------------------------------
    void queue_button_clicked()
    {
      if (m_clicked_func == nullptr)
        return;
      push_event(process_button_clicked);
    }
    // -------------------------------------------------------------------------
    // queue_button_pressed
    // -------------------------------------------------------------------------
    void queue_button_pressed()
    {
      if (m_pressed_func == nullptr)
        return;
      push_event(process_button_pressed);
    }
    // -------------------------------------------------------------------------
    // queue_button_released
    // -------------------------------------------------------------------------
    void queue_button_released()
    {
      if (m_released_func == nullptr)
        return;
      push_event(process_button_released);
    }
    // -------------------------------------------------------------------------
    // process_button_clicked
    // -------------------------------------------------------------------------
    static void process_button_clicked(base::EventData *in_event)
    {
      auto  *button = (ButtonData *)in_event->get_source();
      button->m_clicked_func(button->m_user_data);
    }
    // -------------------------------------------------------------------------
    // process_button_pressed
    // -------------------------------------------------------------------------
    static void process_button_pressed(base::EventData *in_event)
    {
      auto  *button = (ButtonData *)in_event->get_source();
      button->m_pressed_func(button->m_user_data);
    }
    // -------------------------------------------------------------------------
    // process_button_released
    // -------------------------------------------------------------------------
    static void process_button_released(base::EventData *in_event)
    {
      auto  *button = (ButtonData *)in_event->get_source();
      button->m_released_func(button->m_user_data);
    }

  private:
    // member variables --------------------------------------------------------
    Gtk::Button *m_button;
    std::string m_button_str;
    void *m_user_data;
    void (*m_clicked_func)(void *in_user_data);
    void (*m_pressed_func)(void *in_user_data);
    void (*m_released_func)(void *in_user_data);

    friend class WindowData;
    friend class WindowView;
  };

  // ===========================================================================
  //  LabelUpdate class
  // ===========================================================================
  class LabelUpdate : public base::EventData
  {
  protected:
      // member variables --------------------------------------------------------
      bool m_value_updated;
      std::string m_text;

      // -------------------------------------------------------------------------
      // LabelUpdate constructor
      // -------------------------------------------------------------------------
      LabelUpdate(const char *in_text,
                  WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
          m_text(in_text),
          m_value_updated(true),
          base::EventData(in_source_widget, in_handler)
      {
      }
      // -------------------------------------------------------------------------
      // LabelUpdate destructor
      // -------------------------------------------------------------------------
      ~LabelUpdate() override = default;

      friend class LabelData;
  };

  // ===========================================================================
  //  LabelData class (GtkLabel)
  // ===========================================================================
  class LabelData : public WidgetData
  {
  public:
      // Member functions --------------------------------------------------------
      // -------------------------------------------------------------------------
      // get_value
      // -------------------------------------------------------------------------
      bool get_value(std::string *io_text, bool in_compare = false)
      {
        if (io_text == nullptr)
          return false;
        std::lock_guard<std::mutex> lock(m_text_mutex);
        if (in_compare)
          if (*io_text == m_text)
            return false;
        *io_text = m_text;
        return true;
      }
      // -------------------------------------------------------------------------
      // set_value
      // -------------------------------------------------------------------------
      void set_value(const char *in_text, bool in_invoke_update = true)
      {
        if (m_static_text == nullptr)
        {
          m_initial.m_text = in_text;
          return;
        }
        auto *update = new LabelUpdate(in_text, this, process_update);
        push_update(update);
        if (in_invoke_update)
          invoke_update();
      }

  protected:
      // -------------------------------------------------------------------------
      // LabelData constructor
      // -------------------------------------------------------------------------
      LabelData(base::WindowBase *in_window,
                const char *in_label_str,
                const char *in_text,
                base::EventQueue *in_user_event_queue = nullptr) :
          WidgetData(in_window, in_label_str, in_user_event_queue),
          m_initial(in_text, nullptr, nullptr),
          m_static_text(nullptr)
      {
      }
      // -------------------------------------------------------------------------
      // LabelData destructor
      // -------------------------------------------------------------------------
      ~LabelData() override
      {
        delete m_static_text;
      }

      // Member functions --------------------------------------------------------
      // -------------------------------------------------------------------------
      // create
      // -------------------------------------------------------------------------
      Gtk::Box *create() override
      {
        Gtk::Box  *box = WidgetData::create();
        if (box == nullptr)
          return nullptr;
        m_text = m_initial.m_text;
        m_static_text = new Gtk::Label(m_text);
        if (m_static_text == nullptr)
          return nullptr;
        box->pack_end(*m_static_text, Gtk::PACK_SHRINK);
        return box;
      }
      // -------------------------------------------------------------------------
      // process_update
      // -------------------------------------------------------------------------
      static void process_update(base::EventData *in_update)
      {
        auto *update = (LabelUpdate *) in_update;
        auto *label = (LabelData *) update->get_source();

        if (update->m_value_updated)
        {
          label->m_static_text->set_label(update->m_text);
          return;
        }
      }

  private:
      Gtk::Label *m_static_text;
      LabelUpdate  m_initial;

      std::string m_text;
      std::mutex  m_text_mutex;

      friend class WindowData;
      friend class WindowView;
  };

  // ===========================================================================
  //  EntryEvent class
  // ===========================================================================
  class EntryEvent : public base::EventData
  {
  protected:
    // member variables --------------------------------------------------------
    std::string m_text;

    // -------------------------------------------------------------------------
    // EntryEvent constructor
    // -------------------------------------------------------------------------
    EntryEvent(const std::string &in_text,
            WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
            base::EventData(in_source_widget, in_handler),
            m_text(in_text)
    {
    }
    // -------------------------------------------------------------------------
    // EntryEvent destructor
    // -------------------------------------------------------------------------
    ~EntryEvent() override = default;

    friend class EntryData;
  };

  // ===========================================================================
  //  EntryUpdate class
  // ===========================================================================
  class EntryUpdate : public base::EventData
  {
  protected:
      // member variables --------------------------------------------------------
      bool m_value_updated;
      std::string m_text;

      // -------------------------------------------------------------------------
      // EntryUpdate constructor
      // -------------------------------------------------------------------------
      EntryUpdate(const std::string &in_text,
                  WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
          m_text(in_text),
          m_value_updated(true),
          base::EventData(in_source_widget, in_handler)
      {
      }
      // -------------------------------------------------------------------------
      // EntryUpdate destructor
      // -------------------------------------------------------------------------
      ~EntryUpdate() override = default;

      friend class EntryData;
  };

  // ===========================================================================
  //  EntryData class (GtkEntry)
  // ===========================================================================
  class EntryData : public WidgetData
  {
  public:
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // get_value
    // -------------------------------------------------------------------------
    bool get_value(std::string *io_text, bool in_compare = false)
    {
      if (io_text == nullptr)
        return false;
      std::lock_guard<std::mutex> lock(m_text_mutex);
      if (in_compare)
        if (*io_text == m_text)
          return false;
      *io_text = m_text;
      return true;
    }
    // -------------------------------------------------------------------------
    // set_value
    // -------------------------------------------------------------------------
    void set_value(const std::string &in_text, bool in_invoke_update = true)
    {
      if (m_entry == nullptr)
      {
        m_initial.m_text = in_text;
        return;
      }
      auto *update = new EntryUpdate(in_text, this, process_update);
      push_update(update);
      if (in_invoke_update)
        invoke_update();
    }

  protected:
    // -------------------------------------------------------------------------
    // EntryData constructor
    // -------------------------------------------------------------------------
    EntryData(base::WindowBase *in_window,
              const char *in_label_str,
              std::string *in_user_text,
              int in_max_length = -1,
              void *in_user_data = nullptr,
              void (*in_changed_func)(void *, std::string) = nullptr,
              void (*in_done_func)(void *, std::string) = nullptr,
              base::EventQueue *in_user_event_queue = nullptr) :
            WidgetData(in_window, in_label_str, in_user_event_queue),
            m_user_text(in_user_text),
            m_max_length(in_max_length),
            m_initial("", nullptr, nullptr),
            m_user_data(in_user_data),
            m_changed_func(in_changed_func),
            m_done_func(in_done_func),
            m_entry(nullptr)
    {
      if (in_user_text != nullptr)
        m_initial.m_text = *in_user_text;
    }
    // -------------------------------------------------------------------------
    // EntryData destructor
    // -------------------------------------------------------------------------
    ~EntryData() override
    {
      delete m_entry;
    }

    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // create
    // -------------------------------------------------------------------------
    Gtk::Box *create() override
    {
      Gtk::Box  *box = WidgetData::create();
      if (box == nullptr)
        return nullptr;
      m_text = m_initial.m_text;
      m_entry = new Gtk::Entry();
      if (m_entry == nullptr)
        return nullptr;
      if (m_max_length >= 0)
        m_entry->set_max_length(m_max_length);
      m_entry->set_text(m_text);
      m_entry->signal_changed().connect(
              sigc::mem_fun(*this, &EntryData::queue_changed));
      m_entry->signal_editing_done().connect(
              sigc::mem_fun(*this, &EntryData::queue_done));
      box->pack_end(*m_entry, Gtk::PACK_SHRINK);
      return box;
    }
    // -------------------------------------------------------------------------
    // queue_event
    // -------------------------------------------------------------------------
    void queue_event(void (*in_func)(base::EventData *), void (*in_user_func)(void *, std::string))
    {
      std::lock_guard<std::mutex> lock(m_text_mutex);
      m_text = m_entry->get_buffer()->get_text().c_str();
      if (in_user_func == nullptr && m_user_text == nullptr)
        return;
      auto *event = new EntryEvent(
              m_text,
              this, in_func);
      push_event(event);
    }
    // -------------------------------------------------------------------------
    // queue_changed
    // -------------------------------------------------------------------------
    void queue_changed()
    {
      queue_event(process_changed, m_changed_func);
    }
    // -------------------------------------------------------------------------
    // queue_done
    // -------------------------------------------------------------------------
    void queue_done()
    {
      queue_event(process_done, m_done_func);
    }
    // -------------------------------------------------------------------------
    // process_changed
    // -------------------------------------------------------------------------
    static void process_changed(base::EventData *in_event)
    {
      auto *event = (EntryEvent *)in_event;
      auto  *entry = (EntryData *)event->get_source();

      if (entry->m_user_text != nullptr)
        *(entry->m_user_text) = event->m_text;
      if (entry->m_changed_func != nullptr)
        entry->m_changed_func(entry->m_user_data, event->m_text);
    }
    // -------------------------------------------------------------------------
    // process_done
    // -------------------------------------------------------------------------
    static void process_done(base::EventData *in_event)
    {
      auto *event = (EntryEvent *)in_event;
      auto  *entry = (EntryData *)event->get_source();

      if (entry->m_user_text != nullptr)
        *(entry->m_user_text) = event->m_text;
      if (entry->m_changed_func != nullptr)
        entry->m_done_func(entry->m_user_data, event->m_text);
    }
    // -------------------------------------------------------------------------
    // process_update
    // -------------------------------------------------------------------------
    static void process_update(base::EventData *in_update)
    {
      auto *update = (EntryUpdate *) in_update;
      auto *entry = (EntryData *) update->get_source();

      if (update->m_value_updated)
      {
        entry->m_entry->set_text(update->m_text);
        return;
      }
    }

  private:
    Gtk::Entry *m_entry;
    EntryUpdate  m_initial;
    int m_max_length;

    std::string m_text;
    std::mutex  m_text_mutex;

    std::string *m_user_text;
    void *m_user_data;
    void (*m_changed_func)(void *, std::string);
    void (*m_done_func)(void *, std::string);

    friend class WindowData;
    friend class WindowView;
  };

  // ===========================================================================
  //  SpinButtonEvent class
  // ===========================================================================
  class SpinButtonEvent : public base::EventData
  {
  protected:
    // member variables --------------------------------------------------------
    double m_value;

    // -------------------------------------------------------------------------
    // SpinButtonEvent constructor
    // -------------------------------------------------------------------------
    SpinButtonEvent(double in_value,
               WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
            base::EventData(in_source_widget, in_handler),
            m_value(in_value)
    {
    }
    // -------------------------------------------------------------------------
    // SpinButtonEvent destructor
    // -------------------------------------------------------------------------
    ~SpinButtonEvent() override = default;

    friend class SpinButtonData;
  };

  // ===========================================================================
  //  SpinButtonUpdate class
  // ===========================================================================
  class SpinButtonUpdate : public base::EventData
  {
  protected:
    // member variables --------------------------------------------------------
    bool m_value_updated;
    bool m_range_updated, m_increments_updated;
    bool m_wrap_updated, m_numeric_updated;
    bool m_digits_updated;
    bool m_adjustment_called; // value, lower, upper, step, page, page_size
    bool m_configure_called;  // climb_rate, digits
    //
    double m_value;
    double m_lower, m_upper;
    double m_step_increment, m_page_increment;
    double m_page_size, m_climb_rate;
    bool m_wrap, m_numeric;
    guint m_digits;

    // -------------------------------------------------------------------------
    // SpinButtonUpdate constructor
    // -------------------------------------------------------------------------
    SpinButtonUpdate(double in_value,
                     double in_lower, double in_upper,
                     double in_step_increment,
                     double in_page_increment,
                     double in_page_size,
                     double in_climb_rate,
                     guint in_digits,
                     bool in_wrap, bool in_numeric) :
            m_value(in_value),
            m_lower(in_lower), m_upper(in_upper),
            m_step_increment(in_step_increment), m_page_increment(in_page_increment),
            m_page_size(in_page_size), m_climb_rate(in_climb_rate),
            m_digits(in_digits),
            m_wrap(in_wrap), m_numeric(in_numeric),
            m_value_updated(true),
            m_range_updated(true), m_increments_updated(true),
            m_digits_updated(true),
            m_wrap_updated(true), m_numeric_updated(true),
            m_adjustment_called(true),
            m_configure_called(true),
            base::EventData(nullptr, nullptr)
    {
    }
    // -------------------------------------------------------------------------
    // SpinButtonUpdate constructor
    // -------------------------------------------------------------------------
    SpinButtonUpdate(WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
            m_value(0),
            m_lower(0), m_upper(0),
            m_step_increment(1), m_page_increment(10),
            m_page_size(0), m_climb_rate(0),
            m_digits(0),
            m_wrap(false), m_numeric(false),
            m_value_updated(false),
            m_range_updated(false), m_increments_updated(false),
            m_digits_updated(false),
            m_wrap_updated(false), m_numeric_updated(false),
            m_adjustment_called(false),
            m_configure_called(false),
            base::EventData(in_source_widget, in_handler)
    {
    }
    // -------------------------------------------------------------------------
    // SpinButtonUpdate destructor
    // -------------------------------------------------------------------------
    ~SpinButtonUpdate() override = default;

    friend class SpinButtonData;
  };

  // ===========================================================================
  //  SpinButtonData class (GtkSpinButton)
  // ===========================================================================
  class SpinButtonData : public WidgetData
  {
  public:
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // get_value
    // -------------------------------------------------------------------------
    bool get_value(double *io_value, bool in_compare = false)
    {
      if (io_value == nullptr)
        return false;
      std::lock_guard<std::mutex> lock(m_value_mutex);
      if (in_compare)
        if (*io_value == m_value)
          return false;
      *io_value = m_value;
      return true;
    }
    // -------------------------------------------------------------------------
    // set_value
    // -------------------------------------------------------------------------
    void set_value(double in_value, bool in_invoke_update = true)
    {
      if (m_spin == nullptr)
      {
        m_initial.m_value = in_value;
        return;
      }
      auto *update = new SpinButtonUpdate(this, process_update);
      update->m_value = in_value;
      update->m_value_updated = true;
      push_update(update);
      if (in_invoke_update)
        invoke_update();
    }
    // -------------------------------------------------------------------------
    // set_adjustment
    // -------------------------------------------------------------------------
    void set_adjustment(double in_value,
                        double in_lower, double in_upper,
                        double in_step_increment = 1,
                        double in_page_increment = 10,
                        double in_page_size = 0,
                        bool in_invoke_update = true)
    {
      if (m_spin == nullptr)
      {
        m_initial.m_value = in_value;
        m_initial.m_lower = in_lower;
        m_initial.m_upper = in_upper;
        m_initial.m_step_increment = in_step_increment;
        m_initial.m_page_increment = in_page_increment;
        m_initial.m_page_size = in_page_size;
        return;
      }
      auto *update = new SpinButtonUpdate(this, process_update);
      update->m_value = in_value;
      update->m_lower = in_lower;
      update->m_upper = in_upper;
      update->m_step_increment = in_step_increment;
      update->m_page_increment = in_page_increment;
      update->m_page_size = in_page_size;
      update->m_adjustment_called = true;
      push_update(update);
      if (in_invoke_update)
        invoke_update();
    }
    // -------------------------------------------------------------------------
    // set_configure
    // -------------------------------------------------------------------------
    void set_configure(double in_climb_rate, guint in_digits, bool in_invoke_update = true)
    {
      if (m_spin == nullptr)
      {
        m_initial.m_climb_rate = in_climb_rate;
        m_initial.m_digits = in_digits;
        return;
      }
      auto *update = new SpinButtonUpdate(this, process_update);
      update->m_climb_rate = in_climb_rate;
      update->m_digits = in_digits;
      update->m_configure_called = true;
      push_update(update);
      if (in_invoke_update)
        invoke_update();
    }
    // -------------------------------------------------------------------------
    // set_range
    // -------------------------------------------------------------------------
    void set_range(double in_lower, guint in_upper, bool in_invoke_update = true)
    {
      if (m_spin == nullptr)
      {
        m_initial.m_lower = in_lower;
        m_initial.m_upper = in_upper;
        return;
      }
      auto *update = new SpinButtonUpdate(this, process_update);
      update->m_lower = in_lower;
      update->m_upper = in_upper;
      update->m_range_updated = true;
      push_update(update);
      if (in_invoke_update)
        invoke_update();
    }
    // -------------------------------------------------------------------------
    // set_increments
    // -------------------------------------------------------------------------
    void set_increments(double in_step_increment, guint in_page_increment, bool in_invoke_update = true)
    {
      if (m_spin == nullptr)
      {
        m_initial.m_step_increment = in_step_increment;
        m_initial.m_page_increment = in_page_increment;
        return;
      }
      auto *update = new SpinButtonUpdate(this, process_update);
      update->m_step_increment = in_step_increment;
      update->m_page_increment = in_page_increment;
      update->m_increments_updated = true;
      push_update(update);
      if (in_invoke_update)
        invoke_update();
    }
    // -------------------------------------------------------------------------
    // set_digits
    // -------------------------------------------------------------------------
    void set_digits(guint in_digits, bool in_invoke_update = true)
    {
      if (m_spin == nullptr)
      {
        m_initial.m_digits = in_digits;
        return;
      }
      auto *update = new SpinButtonUpdate(this, process_update);
      update->m_digits = in_digits;
      update->m_digits_updated = true;
      push_update(update);
      if (in_invoke_update)
        invoke_update();
    }
    // -------------------------------------------------------------------------
    // set_wrap
    // -------------------------------------------------------------------------
    void set_wrap(bool in_wrap, bool in_invoke_update = true)
    {
      if (m_spin == nullptr)
      {
        m_initial.m_value = in_wrap;
        return;
      }
      auto *update = new SpinButtonUpdate(this, process_update);
      update->m_wrap = in_wrap;
      update->m_wrap_updated = true;
      push_update(update);
      if (in_invoke_update)
        invoke_update();
    }
    // -------------------------------------------------------------------------
    // set_numeric
    // -------------------------------------------------------------------------
    void set_numeric(bool in_numeric, bool in_invoke_update = true)
    {
      if (m_spin == nullptr)
      {
        m_initial.m_numeric = in_numeric;
        return;
      }
      auto *update = new SpinButtonUpdate(this, process_update);
      update->m_numeric = in_numeric;
      update->m_numeric_updated = true;
      push_update(update);
      if (in_invoke_update)
        invoke_update();
    }

  protected:
    // -------------------------------------------------------------------------
    // SpinButtonData constructor
    // -------------------------------------------------------------------------
    SpinButtonData( base::WindowBase *in_window,
                    const char *in_label_str,
                    double *in_user_variable,
                    double in_lower, double in_upper,
                    double in_step_increment = 1,
                    double in_page_increment = 10,
                    double in_page_size = 0,
                    double in_climb_rate = 0,
                    guint in_digits = 0,
                    void *in_user_data = nullptr,
                    void (*in_value_changed_func)(void *, double) = nullptr,
                    bool in_wrap = false, bool in_numeric = false,
                    base::EventQueue *in_user_event_queue = nullptr) :
        WidgetData(in_window, in_label_str, in_user_event_queue),
        m_user_variable(in_user_variable),
        m_initial(0, in_lower, in_upper, in_step_increment,
                      in_page_increment, in_page_size, in_climb_rate,
                      in_digits, in_wrap, in_numeric),
        m_user_data(in_user_data),
        m_value_changed_func(in_value_changed_func),
        m_spin(nullptr), m_value(0)
    {
      if (in_user_variable != nullptr)
        m_initial.m_value = *in_user_variable;
    }
    // -------------------------------------------------------------------------
    // SpinButtonData destructor
    // -------------------------------------------------------------------------
    ~SpinButtonData() override
    {
      delete m_spin;
    }

    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // create
    // -------------------------------------------------------------------------
    Gtk::Box *create() override
    {
      Gtk::Box  *box = WidgetData::create();
      if (box == nullptr)
        return nullptr;
      if (m_user_variable != nullptr)
        *m_user_variable = m_initial.m_value;
      m_value = m_initial.m_value;
      m_spin = new Gtk::SpinButton(
        Gtk::Adjustment::create(
                m_value,
                m_initial.m_lower, m_initial.m_upper,
                m_initial.m_step_increment,
                m_initial.m_page_increment, m_initial.m_page_size),
        m_initial.m_climb_rate, m_initial.m_digits);
      if (m_spin == nullptr)
        return nullptr;
      m_spin->set_wrap(m_initial.m_wrap);
      m_spin->set_numeric(m_initial.m_numeric);
      m_spin->signal_changed().connect(
              sigc::mem_fun(*this, &SpinButtonData::queue_value_changed));
      box->pack_end(*m_spin, Gtk::PACK_SHRINK);
      return box;
    }
    // -------------------------------------------------------------------------
    // queue_value_changed
    // -------------------------------------------------------------------------
    void queue_value_changed()
    {
      std::lock_guard<std::mutex> lock(m_value_mutex);
      m_value = m_spin->get_value();
      mark_as_updated();
      if (m_value_changed_func == nullptr && m_user_variable == nullptr)
        return;
      auto *event = new SpinButtonEvent(
              m_value,
              this, process_value_changed);
      push_event(event);
    }
    // -------------------------------------------------------------------------
    // process_value_changed
    // -------------------------------------------------------------------------
    static void process_value_changed(base::EventData *in_event)
    {
      auto *event = (SpinButtonEvent *)in_event;
      auto  *spin = (SpinButtonData *)event->get_source();

      if (spin->m_user_variable != nullptr)
        *(spin->m_user_variable) = event->m_value;
      if (spin->m_value_changed_func != nullptr)
        spin->m_value_changed_func(spin->m_user_data, event->m_value);
    }
    // -------------------------------------------------------------------------
    // process_update
    // -------------------------------------------------------------------------
    static void process_update(base::EventData *in_update)
    {
      auto *update = (SpinButtonUpdate *)in_update;
      auto  *spin = (SpinButtonData *)update->get_source();

      if (update->m_value_updated)
      {
        spin->m_spin->set_value(update->m_value);
        return;
      }
      if (update->m_adjustment_called)
      {
        spin->m_spin->set_adjustment(Gtk::Adjustment::create(
                update->m_value,
                update->m_lower, update->m_upper,
                update->m_step_increment,
                update->m_page_increment, update->m_page_size));
        return;
      }
      if (update->m_configure_called)
      {
        spin->m_spin->configure(Glib::RefPtr<Gtk::Adjustment>(nullptr),
                                update->m_climb_rate, update->m_digits);
        return;
      }
      if (update->m_range_updated)
      {
        spin->m_spin->set_range(update->m_lower,
                                update->m_upper);
        return;
      }
      if (update->m_increments_updated)
      {
        spin->m_spin->set_increments(update->m_step_increment,
                                     update->m_page_increment);
        return;
      }
      if (update->m_digits_updated)
      {
        spin->m_spin->set_digits(update->m_digits);
        return;
      }
      if (update->m_wrap_updated)
      {
        spin->m_spin->set_wrap(update->m_wrap);
        return;
      }
      if (update->m_numeric_updated)
      {
        spin->m_spin->set_numeric(update->m_numeric);
        return;
      }
    }

  private:
    Gtk::SpinButton *m_spin;
    SpinButtonUpdate  m_initial;

    double m_value;
    std::mutex  m_value_mutex;

    double *m_user_variable;
    void *m_user_data;
    void (*m_value_changed_func)(void *, double);

    friend class WindowData;
    friend class WindowView;
  };

  // ===========================================================================
  //  SwitchEvent class
  // ===========================================================================
  class SwitchEvent : public base::EventData
  {
  protected:
      // member variables --------------------------------------------------------
      bool m_value;

      // -------------------------------------------------------------------------
      // SwitchEvent constructor
      // -------------------------------------------------------------------------
      SwitchEvent(bool in_value,
                  WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
          base::EventData(in_source_widget, in_handler),
          m_value(in_value)
      {
      }
      // -------------------------------------------------------------------------
      // SwitchEvent destructor
      // -------------------------------------------------------------------------
      ~SwitchEvent() override = default;

      friend class SwitchData;
  };

  // ===========================================================================
  //  SwitchUpdate class
  // ===========================================================================
  class SwitchUpdate : public base::EventData
  {
  protected:
      // member variables --------------------------------------------------------
      bool m_value_updated;
      bool m_value;

      // -------------------------------------------------------------------------
      // SwitchUpdate constructor
      // -------------------------------------------------------------------------
      SwitchUpdate(bool in_value,
                   WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
          m_value(in_value),
          m_value_updated(true),
          base::EventData(in_source_widget, in_handler)
      {
      }
      // -------------------------------------------------------------------------
      // SwitchUpdate destructor
      // -------------------------------------------------------------------------
      ~SwitchUpdate() override = default;

      friend class SwitchData;
  };

  // ===========================================================================
  //  SwitchData class (GtkSwitch)
  // ===========================================================================
  class SwitchData : public WidgetData
  {
  public:
      // Member functions --------------------------------------------------------
      // -------------------------------------------------------------------------
      // get_value
      // -------------------------------------------------------------------------
      bool get_value(bool *io_value, bool in_compare = false)
      {
        if (io_value == nullptr)
          return false;
        std::lock_guard<std::mutex> lock(m_value_mutex);
        if (in_compare)
          if (*io_value == m_value)
            return false;
        *io_value = m_value;
        return true;
      }
      // -------------------------------------------------------------------------
      // set_value
      // -------------------------------------------------------------------------
      void set_value(bool in_value, bool in_invoke_update = true)
      {
        if (m_switch == nullptr)
        {
          m_initial.m_value = in_value;
          return;
        }
        auto *update = new SwitchUpdate(in_value, this, process_update);
        push_update(update);
        if (in_invoke_update)
          invoke_update();
      }

  protected:
      // -------------------------------------------------------------------------
      // SwitchData destructor
      // -------------------------------------------------------------------------
      ~SwitchData() override
      {
        delete m_switch;
      }

      // -------------------------------------------------------------------------
      // SwitchData constructor
      // -------------------------------------------------------------------------
      SwitchData(base::WindowBase *in_window,
                 const char *in_label_str,
                 bool *in_user_variable,
                 void *in_user_data = nullptr,
                 void (*in_toggled_func)(void *in_user_data) = nullptr,
                 base::EventQueue *in_user_event_queue = nullptr) :
          WidgetData(in_window, in_label_str, in_user_event_queue),
          m_user_variable(in_user_variable),
          m_initial(false, nullptr, nullptr),
          m_user_data(in_user_data),
          m_toggled_func(in_toggled_func),
          m_switch(nullptr), m_value(false)
      {
        if (in_user_variable != nullptr)
          m_initial.m_value = *in_user_variable;
      }
      // Member functions --------------------------------------------------------
      // -------------------------------------------------------------------------
      // create
      // -------------------------------------------------------------------------
      Gtk::Box *create() override
      {
        Gtk::Box  *box = WidgetData::create();
        if (box == nullptr)
          return nullptr;
        if (m_user_variable != nullptr)
          *m_user_variable = m_initial.m_value;
        m_value = m_initial.m_value;
        m_switch = new Gtk::Switch();
        if (m_switch == nullptr)
          return nullptr;
        m_switch->set_active(m_value);
        m_switch->signal_state_set().connect(
            sigc::mem_fun(*this, &SwitchData::queue_state_set));
        box->pack_end(*m_switch, Gtk::PACK_SHRINK);
        return box;
      }
      // -------------------------------------------------------------------------
      // queue_state_set
      // -------------------------------------------------------------------------
      bool queue_state_set(bool inState)
      {
        std::lock_guard<std::mutex> lock(m_value_mutex);
        m_value = inState;
        mark_as_updated();
        if (m_toggled_func == nullptr && m_user_variable == nullptr)
          return false;
        auto *event = new SwitchEvent(
            m_value,
            this, process_state_set);
        push_event(event);
        return false;
      }
      // -------------------------------------------------------------------------
      // process_state_set
      // -------------------------------------------------------------------------
      static void process_state_set(base::EventData *in_event)
      {
        auto *event = (SwitchEvent *)in_event;
        auto  *button = (SwitchData *)event->get_source();
        if (button->m_user_variable != nullptr)
          *(button->m_user_variable) = event->m_value;
        if (button->m_toggled_func != nullptr)
          button->m_toggled_func(button->m_user_data);
      }
      // -------------------------------------------------------------------------
      // process_update
      // -------------------------------------------------------------------------
      static void process_update(base::EventData *in_update)
      {
        auto *update = (SwitchUpdate *) in_update;
        auto *button = (SwitchData *) update->get_source();

        if (update->m_value_updated)
        {
          button->m_switch->set_active(update->m_value);
          return;
        }
      }

  private:
      // member variables --------------------------------------------------------
      Gtk::Switch *m_switch;
      SwitchUpdate  m_initial;

      bool m_value;
      std::mutex  m_value_mutex;

      bool *m_user_variable;
      void *m_user_data;
      void (*m_toggled_func)(void *in_user_data);

      friend class WindowData;
      friend class WindowView;
  };

  // ===========================================================================
  //  ComboBoxEvent class
  // ===========================================================================
  class ComboBoxEvent : public base::EventData
  {
  protected:
      // member variables --------------------------------------------------------
      int m_value;

      // -------------------------------------------------------------------------
      // ComboBoxEvent constructor
      // -------------------------------------------------------------------------
      ComboBoxEvent(int in_value,
                    WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
          base::EventData(in_source_widget, in_handler),
          m_value(in_value)
      {
      }
      // -------------------------------------------------------------------------
      // ComboBoxEvent destructor
      // -------------------------------------------------------------------------
      ~ComboBoxEvent() override = default;

      friend class ComboBoxData;
  };

  // ===========================================================================
  //  ComboBoxUpdate class
  // ===========================================================================
  class ComboBoxUpdate : public base::EventData
  {
  protected:
      // member variables --------------------------------------------------------
      bool m_value_updated;
      int m_value;

      // -------------------------------------------------------------------------
      // ComboBoxUpdate constructor
      // -------------------------------------------------------------------------
      ComboBoxUpdate(int in_value,
                     WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
          m_value(in_value),
          m_value_updated(true),
          base::EventData(in_source_widget, in_handler)
      {
      }
      // -------------------------------------------------------------------------
      // ComboBoxUpdate destructor
      // -------------------------------------------------------------------------
      ~ComboBoxUpdate() override = default;

      friend class ComboBoxData;
  };

  // ===========================================================================
  //  ComboBoxData class (GtkComboBox)
  // ===========================================================================
  class ComboBoxData : public WidgetData
  {
  public:
      // Member functions --------------------------------------------------------
      // -------------------------------------------------------------------------
      // get_value
      // -------------------------------------------------------------------------
      bool get_value(int *io_value, bool in_compare = false)
      {
        if (io_value == nullptr)
          return false;
        std::lock_guard<std::mutex> lock(m_value_mutex);
        if (in_compare)
          if (*io_value == m_value)
            return false;
        *io_value = m_value;
        return true;
      }
      // -------------------------------------------------------------------------
      // set_value
      // -------------------------------------------------------------------------
      void set_value(int in_value, bool in_invoke_update = true)
      {
        if (m_combo_box == nullptr)
        {
          m_initial.m_value = in_value;
          return;
        }
        auto *update = new ComboBoxUpdate(in_value, this, process_update);
        push_update(update);
        if (in_invoke_update)
          invoke_update();
      }

  protected:
      // -------------------------------------------------------------------------
      // ComboBoxData destructor
      // -------------------------------------------------------------------------
      ~ComboBoxData() override
      {
        delete m_combo_box;
      }

      // -------------------------------------------------------------------------
      // ComboBoxData constructor
      // -------------------------------------------------------------------------
      ComboBoxData(base::WindowBase *in_window,
                   const char *in_label_str,
                   const std::vector<std::string> &in_items,
                   int *in_user_variable,
                   void *in_user_data = nullptr,
                   void (*in_changed_func)(void *in_user_data) = nullptr,
                   base::EventQueue *in_user_event_queue = nullptr) :
          WidgetData(in_window, in_label_str, in_user_event_queue),
          m_user_variable(in_user_variable),
          m_items(in_items),
          m_initial(0, nullptr, nullptr),
          m_user_data(in_user_data),
          m_changed_func(in_changed_func),
          m_combo_box(nullptr), m_value(0)
      {
        if (in_user_variable != nullptr)
          m_initial.m_value = *in_user_variable;
      }
      // Member functions --------------------------------------------------------
      // -------------------------------------------------------------------------
      // create
      // -------------------------------------------------------------------------
      Gtk::Box *create() override
      {
        Gtk::Box  *box = WidgetData::create();
        if (box == nullptr)
          return nullptr;
        if (m_user_variable != nullptr)
          *m_user_variable = m_initial.m_value;
        m_value = m_initial.m_value;
        m_combo_box = new Gtk::ComboBoxText();
        if (m_combo_box == nullptr)
          return nullptr;
        for (auto it = m_items.begin(); it != m_items.end(); it++)
          m_combo_box->append((*it));
        m_combo_box->set_active(m_value);
        m_combo_box->signal_changed().connect(
            sigc::mem_fun(*this, &ComboBoxData::queue_changed));
        box->pack_end(*m_combo_box, Gtk::PACK_SHRINK);
        return box;
      }
      // -------------------------------------------------------------------------
      // queue_changed
      // -------------------------------------------------------------------------
      void queue_changed()
      {
        std::lock_guard<std::mutex> lock(m_value_mutex);
        m_value = m_combo_box->get_active_row_number();
        mark_as_updated();
        if (m_changed_func == nullptr && m_user_variable == nullptr)
          return;
        auto *event = new ComboBoxEvent(
            m_value,
            this, process_changed);
        push_event(event);
      }
      // -------------------------------------------------------------------------
      // process_changed
      // -------------------------------------------------------------------------
      static void process_changed(base::EventData *in_event)
      {
        auto *event = (ComboBoxEvent *)in_event;
        auto  *button = (ComboBoxData *)event->get_source();
        if (button->m_user_variable != nullptr)
          *(button->m_user_variable) = event->m_value;
        if (button->m_changed_func != nullptr)
          button->m_changed_func(button->m_user_data);
      }
      // -------------------------------------------------------------------------
      // process_update
      // -------------------------------------------------------------------------
      static void process_update(base::EventData *in_update)
      {
        auto *update = (ComboBoxUpdate *) in_update;
        auto *button = (ComboBoxData *) update->get_source();

        if (update->m_value_updated)
        {
          button->m_combo_box->set_active(update->m_value);
          return;
        }
      }

  private:
      // member variables --------------------------------------------------------
      Gtk::ComboBoxText *m_combo_box;
      ComboBoxUpdate  m_initial;

      int m_value;
      std::mutex  m_value_mutex;
      std::vector<std::string>  m_items;

      int *m_user_variable;
      void *m_user_data;
      void (*m_changed_func)(void *in_user_data);

      friend class WindowData;
      friend class WindowView;
  };

  // ===========================================================================
  //  ScaleEvent class
  // ===========================================================================
  class ScaleEvent : public base::EventData
  {
  protected:
      // member variables --------------------------------------------------------
      double m_value;

      // -------------------------------------------------------------------------
      // ScaleEvent constructor
      // -------------------------------------------------------------------------
      ScaleEvent(double in_value,
                 WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
          base::EventData(in_source_widget, in_handler),
          m_value(in_value)
      {
      }
      // -------------------------------------------------------------------------
      // ScaleEvent destructor
      // -------------------------------------------------------------------------
      ~ScaleEvent() override = default;

      friend class ScaleData;
  };

  // ===========================================================================
  //  ScaleUpdate class
  // ===========================================================================
  class ScaleUpdate : public base::EventData
  {
  protected:
      // member variables --------------------------------------------------------
      bool m_value_updated;
      bool m_range_updated, m_increments_updated;
      bool m_digits_updated;
      bool m_adjustment_called; // value, lower, upper, step, page, page_size
      //
      double m_value;
      double m_lower, m_upper;
      double m_step_increment, m_page_increment;
      double m_page_size;
      int m_digits;

      // -------------------------------------------------------------------------
      // ScaleUpdate constructor
      // -------------------------------------------------------------------------
      ScaleUpdate(double in_value,
                  double in_lower, double in_upper,
                  double in_step_increment,
                  double in_page_increment,
                  double in_page_size,
                  int in_digits) :
          m_value(in_value),
          m_lower(in_lower), m_upper(in_upper),
          m_step_increment(in_step_increment), m_page_increment(in_page_increment),
          m_page_size(in_page_size),
          m_digits(in_digits),
          m_value_updated(true),
          m_range_updated(true), m_increments_updated(true),
          m_digits_updated(true),
          m_adjustment_called(true),
          base::EventData(nullptr, nullptr)
      {
      }
      // -------------------------------------------------------------------------
      // ScaleUpdate constructor
      // -------------------------------------------------------------------------
      ScaleUpdate(WidgetData *in_source_widget, void (*in_handler)(base::EventData *)) :
          m_value(0),
          m_lower(0), m_upper(0),
          m_step_increment(1), m_page_increment(10),
          m_page_size(0),
          m_digits(0),
          m_value_updated(false),
          m_range_updated(false), m_increments_updated(false),
          m_digits_updated(false),
          m_adjustment_called(false),
          base::EventData(in_source_widget, in_handler)
      {
      }
      // -------------------------------------------------------------------------
      // ScaleUpdate destructor
      // -------------------------------------------------------------------------
      ~ScaleUpdate() override = default;

      friend class ScaleData;
  };

  // ===========================================================================
  //  ScaleData class (GtkScale)
  // ===========================================================================
  class ScaleData : public WidgetData
  {
  public:
      // Member functions --------------------------------------------------------
      // -------------------------------------------------------------------------
      // get_value
      // -------------------------------------------------------------------------
      bool get_value(double *io_value, bool in_compare = false)
      {
        if (io_value == nullptr)
          return false;
        std::lock_guard<std::mutex> lock(m_value_mutex);
        if (in_compare)
          if (*io_value == m_value)
            return false;
        *io_value = m_value;
        return true;
      }
      // -------------------------------------------------------------------------
      // set_value
      // -------------------------------------------------------------------------
      void set_value(double in_value, bool in_invoke_update = true)
      {
        if (m_scale == nullptr)
        {
          m_initial.m_value = in_value;
          return;
        }
        auto *update = new ScaleUpdate(this, process_update);
        update->m_value = in_value;
        update->m_value_updated = true;
        push_update(update);
        if (in_invoke_update)
          invoke_update();
      }
      // -------------------------------------------------------------------------
      // set_adjustment
      // -------------------------------------------------------------------------
      void set_adjustment(double in_value,
                          double in_lower, double in_upper,
                          double in_step_increment = 1,
                          double in_page_increment = 10,
                          double in_page_size = 0,
                          bool in_invoke_update = true)
      {
        if (m_scale == nullptr)
        {
          m_initial.m_value = in_value;
          m_initial.m_lower = in_lower;
          m_initial.m_upper = in_upper;
          m_initial.m_step_increment = in_step_increment;
          m_initial.m_page_increment = in_page_increment;
          m_initial.m_page_size = in_page_size;
          return;
        }
        auto *update = new ScaleUpdate(this, process_update);
        update->m_value = in_value;
        update->m_lower = in_lower;
        update->m_upper = in_upper;
        update->m_step_increment = in_step_increment;
        update->m_page_increment = in_page_increment;
        update->m_page_size = in_page_size;
        update->m_adjustment_called = true;
        push_update(update);
        if (in_invoke_update)
          invoke_update();
      }
      // -------------------------------------------------------------------------
      // set_digits
      // -------------------------------------------------------------------------
      void set_digits(int in_digits, bool in_invoke_update = true)
      {
        if (m_scale == nullptr)
        {
          m_initial.m_digits = in_digits;
          return;
        }
        auto *update = new ScaleUpdate(this, process_update);
        update->m_digits = in_digits;
        update->m_digits_updated = true;
        push_update(update);
        if (in_invoke_update)
          invoke_update();
      }
      // -------------------------------------------------------------------------
      // set_range
      // -------------------------------------------------------------------------
      void set_range(double in_lower, guint in_upper, bool in_invoke_update = true)
      {
        if (m_scale == nullptr)
        {
          m_initial.m_lower = in_lower;
          m_initial.m_upper = in_upper;
          return;
        }
        auto *update = new ScaleUpdate(this, process_update);
        update->m_lower = in_lower;
        update->m_upper = in_upper;
        update->m_range_updated = true;
        push_update(update);
        if (in_invoke_update)
          invoke_update();
      }
      // -------------------------------------------------------------------------
      // set_increments
      // -------------------------------------------------------------------------
      void set_increments(double in_step_increment, guint in_page_increment, bool in_invoke_update = true)
      {
        if (m_scale == nullptr)
        {
          m_initial.m_step_increment = in_step_increment;
          m_initial.m_page_increment = in_page_increment;
          return;
        }
        auto *update = new ScaleUpdate(this, process_update);
        update->m_step_increment = in_step_increment;
        update->m_page_increment = in_page_increment;
        update->m_increments_updated = true;
        push_update(update);
        if (in_invoke_update)
          invoke_update();
      }

  protected:
      // -------------------------------------------------------------------------
      // ScaleData constructor
      // -------------------------------------------------------------------------
      ScaleData( base::WindowBase *in_window,
                 const char *in_label_str,
                 double *in_user_variable,
                 double in_lower, double in_upper,
                 double in_step_increment = 1,
                 double in_page_increment = 10,
                 double in_page_size = 0,
                 int in_digits = 0,
                 void *in_user_data = nullptr,
                 void (*in_value_changed_func)(void *, double) = nullptr,
                 base::EventQueue *in_user_event_queue = nullptr) :
          WidgetData(in_window, in_label_str, in_user_event_queue),
          m_user_variable(in_user_variable),
          m_initial(0, in_lower, in_upper, in_step_increment,
                    in_page_increment, in_page_size, in_digits),
          m_user_data(in_user_data),
          m_value_changed_func(in_value_changed_func),
          m_scale(nullptr), m_value(0)
      {
        if (in_user_variable != nullptr)
          m_initial.m_value = *in_user_variable;
      }
      // -------------------------------------------------------------------------
      // ScaleData destructor
      // -------------------------------------------------------------------------
      ~ScaleData() override
      {
        delete m_scale;
      }

      // Member functions --------------------------------------------------------
      // -------------------------------------------------------------------------
      // create
      // -------------------------------------------------------------------------
      Gtk::Box *create() override
      {
        Gtk::Box  *box = WidgetData::create();
        if (box == nullptr)
          return nullptr;
        if (m_user_variable != nullptr)
          *m_user_variable = m_initial.m_value;
        m_value = m_initial.m_value;
        m_scale = new Gtk::Scale(
            Gtk::Adjustment::create(
                m_value,
                m_initial.m_lower, m_initial.m_upper,
                m_initial.m_step_increment,
                m_initial.m_page_increment, m_initial.m_page_size));
        if (m_scale == nullptr)
          return nullptr;
        m_scale->set_digits(m_initial.m_digits);
        m_scale->signal_value_changed().connect(
            sigc::mem_fun(*this, &ScaleData::queue_value_changed));
        box->pack_end(*m_scale, Gtk::PACK_EXPAND_WIDGET);
        return box;
      }
      // -------------------------------------------------------------------------
      // queue_value_changed
      // -------------------------------------------------------------------------
      void queue_value_changed()
      {
        std::lock_guard<std::mutex> lock(m_value_mutex);
        m_value = m_scale->get_value();
        mark_as_updated();
        if (m_value_changed_func == nullptr && m_user_variable == nullptr)
          return;
        auto *event = new ScaleEvent(
            m_value,
            this, process_value_changed);
        push_event(event);
      }
      // -------------------------------------------------------------------------
      // process_value_changed
      // -------------------------------------------------------------------------
      static void process_value_changed(base::EventData *in_event)
      {
        auto *event = (ScaleEvent *)in_event;
        auto  *spin = (ScaleData *)event->get_source();

        if (spin->m_user_variable != nullptr)
          *(spin->m_user_variable) = event->m_value;
        if (spin->m_value_changed_func != nullptr)
          spin->m_value_changed_func(spin->m_user_data, event->m_value);
      }
      // -------------------------------------------------------------------------
      // process_update
      // -------------------------------------------------------------------------
      static void process_update(base::EventData *in_update)
      {
        auto *update = (ScaleUpdate *)in_update;
        auto  *spin = (ScaleData *)update->get_source();

        if (update->m_value_updated)
        {
          spin->m_scale->set_value(update->m_value);
          return;
        }
        if (update->m_adjustment_called)
        {
          spin->m_scale->set_adjustment(Gtk::Adjustment::create(
              update->m_value,
              update->m_lower, update->m_upper,
              update->m_step_increment,
              update->m_page_increment, update->m_page_size));
          return;
        }
        if (update->m_digits_updated)
        {
          spin->m_scale->set_digits(update->m_digits);
          return;
        }
        if (update->m_range_updated)
        {
          spin->m_scale->set_range(update->m_lower,
                                   update->m_upper);
          return;
        }
        if (update->m_increments_updated)
        {
          spin->m_scale->set_increments(update->m_step_increment,
                                        update->m_page_increment);
          return;
        }
      }

  private:
      Gtk::Scale *m_scale;
      ScaleUpdate  m_initial;

      double m_value;
      std::mutex  m_value_mutex;

      double *m_user_variable;
      void *m_user_data;
      void (*m_value_changed_func)(void *, double);

      friend class WindowData;
      friend class WindowView;
  };

  // ===========================================================================
  //  WindowData class
  // ===========================================================================
  class WindowData : public base::WindowBase
  {
  public:
    // -------------------------------------------------------------------------
    // WindowData destructor
    // -------------------------------------------------------------------------
    ~WindowData() override
    {
      delete_all_widgets();
    }
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // add_button
    // -------------------------------------------------------------------------
    ButtonData *add_button(const char *in_label_str,
                   const char *in_button_str,
                   void *in_user_data = nullptr,
                   void (*in_clicked_func)(void *in_user_data) = nullptr,
                   void (*in_pressed_func)(void *in_user_data) = nullptr,
                   void (*in_released_func)(void *in_user_data) = nullptr,
                   base::EventQueue *in_user_event_queue = nullptr)
    {
      ButtonData  *button;
      button = new ButtonData(this,
                              in_label_str,
                              in_button_str,
                              in_user_data,
                              in_clicked_func,
                              in_pressed_func,
                              in_released_func,
                              in_user_event_queue);
      add_widget(button);
      return button;
    }
    // -------------------------------------------------------------------------
    // add_label
    // -------------------------------------------------------------------------
    LabelData *add_label(const char *in_label_str,
                          const char  *in_text,
                         base::EventQueue *in_user_event_queue = nullptr)
    {
      LabelData  *label;
      label = new LabelData(this,
                            in_label_str,
                            in_text,
                            in_user_event_queue);
      add_widget(label);
      return label;
    }
    // -------------------------------------------------------------------------
    // add_entry
    // -------------------------------------------------------------------------
    EntryData *add_entry(const char *in_label_str,
                  std::string *in_entry_text,
                  int in_max_length = -1,
                  void *in_user_data = nullptr,
                  void (*in_changed_func)(void *in_user_data, std::string in_string) = nullptr,
                  void (*in_done_func)(void *in_user_data, std::string in_string) = nullptr,
                  base::EventQueue *in_user_event_queue = nullptr)
    {
      EntryData  *entry;
      entry = new EntryData(this,
                            in_label_str,
                            in_entry_text,
                            in_max_length,
                            in_user_data,
                            in_changed_func,
                            in_done_func,
                            in_user_event_queue);
      add_widget(entry);
      return entry;
    }
    // -------------------------------------------------------------------------
    // add_spin_button
    // -------------------------------------------------------------------------
    SpinButtonData *add_spin_button(const char *in_label_str,
                                    double *in_user_variable,
                                    double in_lower, double in_upper,
                                    double in_step_increment = 1,
                                    double in_page_increment = 10,
                                    double in_page_size = 0,
                                    double in_climb_rate = 0,
                                    guint in_digits = 0,
                                    void *in_user_data = nullptr,
                                    void (*in_value_changed_func)(void *, double) = nullptr,
                                    bool in_wrap = false, bool in_numeric = false,
                                    base::EventQueue *in_user_event_queue = nullptr)
    {
      SpinButtonData  *spin;
      spin = new SpinButtonData(this,
                                in_label_str,
                                in_user_variable,
                                in_lower, in_upper,
                                in_step_increment,
                                in_page_increment,
                                in_page_size,
                                in_climb_rate,
                                in_digits,
                                in_user_data,
                                in_value_changed_func,
                                in_wrap, in_numeric,
                                in_user_event_queue);
      add_widget(spin);
      return spin;
    }
    // -------------------------------------------------------------------------
    // add_switch
    // -------------------------------------------------------------------------
    SwitchData *add_switch(const char *in_label_str,
                           bool *in_user_variable,
                           void *in_user_data = nullptr,
                           void (*in_toggled_func)(void *in_user_data) = nullptr,
                           base::EventQueue *in_user_event_queue = nullptr)
    {
      SwitchData  *button;
      button = new SwitchData(this,
                              in_label_str,
                              in_user_variable,
                              in_user_data,
                              in_toggled_func,
                              in_user_event_queue);
      add_widget(button);
      return button;
    }
    // -------------------------------------------------------------------------
    // add_combo_box
    // -------------------------------------------------------------------------
    ComboBoxData *add_combo_box(const char *in_label_str,
                           const std::vector<std::string> &in_items,
                           int *in_user_variable,
                           void *in_user_data = nullptr,
                           void (*in_changed_func)(void *in_user_data) = nullptr,
                           base::EventQueue *in_user_event_queue = nullptr)
    {
      ComboBoxData  *comboBox;
      comboBox = new ComboBoxData(this,
                              in_label_str,
                              in_items,
                              in_user_variable,
                              in_user_data,
                              in_changed_func,
                              in_user_event_queue);
      add_widget(comboBox);
      return comboBox;
    }
    // -------------------------------------------------------------------------
    // add_scale
    // -------------------------------------------------------------------------
    ScaleData *add_scale(const char *in_label_str,
                               double *in_user_variable,
                               double in_lower, double in_upper,
                               double in_step_increment = 1,
                               double in_page_increment = 10,
                               double in_page_size = 0,
                               int    in_digits = 0,
                               void *in_user_data = nullptr,
                               void (*in_value_changed_func)(void *, double) = nullptr,
                               base::EventQueue *in_user_event_queue = nullptr)
    {
      ScaleData  *spin;
      spin = new ScaleData(this,
                           in_label_str,
                           in_user_variable,
                           in_lower, in_upper,
                           in_step_increment,
                           in_page_increment,
                           in_page_size,
                           in_digits,
                           in_user_data,
                           in_value_changed_func,
                           in_user_event_queue);
      add_widget(spin);
      return spin;
    }

  protected:
    // -------------------------------------------------------------------------
    // WindowData constructor
    // -------------------------------------------------------------------------
    WindowData(base::EventQueue *in_user_event_queue = nullptr) :
      base::WindowBase(in_user_event_queue)
    {
    }
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // add_widget
    // -------------------------------------------------------------------------
    void add_widget(WidgetData *in_widget_data)
    {
      if (in_widget_data != nullptr)
        m_widget_list.push_back(in_widget_data);
    }
    // -------------------------------------------------------------------------
    // delete_all_widgets
    // -------------------------------------------------------------------------
    void delete_all_widgets()
    {
      for (auto it = m_widget_list.begin(); it != m_widget_list.end(); it++)
        delete (*it);
    }
    // -------------------------------------------------------------------------
    // get_list
    // -------------------------------------------------------------------------
    std::vector<WidgetData *> *get_list()
    {
      return &m_widget_list;
    }
    // -------------------------------------------------------------------------
    // update (NOTE: need to be called from a GUI thread)
    // -------------------------------------------------------------------------
    void update_widgets()
    {
      // Need to specify in_last_only = false explicitly
      process_update_events(false);
    }

  private:
    // member variables --------------------------------------------------------
    std::vector<WidgetData *>   m_widget_list;

    // friend classes ----------------------------------------------------------
    friend class WindowView;
  };

  // ===========================================================================
  // WindowView class
  // ===========================================================================
  // Note: Order of Scrollable -> Widget is very important
  //
  class WindowView : public Gtk::ScrolledWindow
  {
  protected:
    // -------------------------------------------------------------------------
    // WindowView constructor
    // -------------------------------------------------------------------------
    WindowView(WindowData *in_controls_data) :
            Glib::ObjectBase("WindowView"),
            m_vert_box(Gtk::Orientation::ORIENTATION_VERTICAL),
            m_controls_data(in_controls_data)
    {
      m_vert_box.set_valign(Gtk::ALIGN_START);
      for (auto it = m_controls_data->get_list()->begin(); it != m_controls_data->get_list()->end(); it++)
      {
        Gtk::Box *h_box = (*it)->create();
        if (h_box != nullptr)
          m_vert_box.pack_start(*h_box);
      }
      this->add(m_vert_box);
    }
    // -------------------------------------------------------------------------
    // WindowView destructor
    // -------------------------------------------------------------------------
    ~WindowView() override
    {
      SHL_DBG_OUT("WindowView was deleted");
    }
    // ---------------------------------------------------------------------------
    // update (NOTE: need to be called from a GUI thread)
    // ---------------------------------------------------------------------------
    void update()
    {
      m_controls_data->update_widgets();
    }

  private:
    Gtk::Box m_vert_box;
    WindowData *m_controls_data;

    friend class MainWindow;
  };

  // ---------------------------------------------------------------------------
  //  MainWindow class
  // ---------------------------------------------------------------------------
  class MainWindow : public Gtk::Window
  {
  public:
    // -------------------------------------------------------------------------
    // MainWindow destructor
    // -------------------------------------------------------------------------
    ~MainWindow() override
    {
      SHL_DBG_OUT("MainWindow was deleted");
    }

  protected:
    // -------------------------------------------------------------------------
    // MainWindow constructor
    // -------------------------------------------------------------------------
    MainWindow(WindowData *in_controls_data, const char *in_title) :
        m_header_left_box(Gtk::Orientation::ORIENTATION_HORIZONTAL),
        m_header_right_box(Gtk::Orientation::ORIENTATION_HORIZONTAL),
        m_view(in_controls_data)
    {
      m_action_group = Gio::SimpleActionGroup::create();
      m_action_group->add_action("about",sigc::mem_fun(*this, &MainWindow::on_menu_about));
      insert_action_group("main", m_action_group);
      //
      m_gio_menu = Gio::Menu::create();
      m_gio_menu->append_item(Gio::MenuItem::create("About", "main.about"));
      m_menu_button.set_menu_model(m_gio_menu);
      //
      //
      m_title.set_label(in_title);
      m_menu_button.set_use_popover(true);
      m_menu_button.set_image_from_icon_name("open-menu-symbolic");
      m_header_right_box.pack_end(m_menu_button);
      //
      this->set_titlebar(m_header);
      m_header.set_show_close_button(true);
      m_header.set_custom_title(m_title);
      m_header.pack_start(m_header_left_box);
      m_header.pack_end(m_header_right_box);
      //
      this->add(m_view);

      resize(300, 300);
      show_all_children();
    }
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // on_menu_about
    // -------------------------------------------------------------------------
    void on_menu_about()
    {
      Gtk::AboutDialog  dialog;
      dialog.set_program_name("ControlsWindow-GTK");
      dialog.set_version(SHL_CONTROLS_WINDOW_GTK_BASE_VERSION);
      dialog.set_copyright("Copyright (c) 2022-2024 Dairoku Sekiguchi");
      dialog.set_transient_for(*this);
      dialog.run();
    }
    // ---------------------------------------------------------------------------
    // update (NOTE: need to be called from a GUI thread)
    // ---------------------------------------------------------------------------
    void update()
    {
      m_view.update();
    }

  private:
    // member variables --------------------------------------------------------
    Glib::RefPtr<Gio::SimpleActionGroup> m_action_group;
    Glib::RefPtr<Gio::Menu> m_gio_menu;
    Gtk::Box m_header_left_box;
    Gtk::Label m_title;
    Gtk::MenuButton m_menu_button;
    Gtk::Box m_header_right_box;
    Gtk::HeaderBar m_header;
    //
    WindowView  m_view;

    friend class shl::gtk::ControlsWindow;

    // Member functions ----------------------------------------------------------
  };
};

  // ---------------------------------------------------------------------------
  //  ControlsWindow class
  // ---------------------------------------------------------------------------
  class ControlsWindow :
      public shl::gtk::controls::WindowData
  {
  public:
    // constructors and destructor ---------------------------------------------
    // -------------------------------------------------------------------------
    // ControlsWindow
    // -------------------------------------------------------------------------
    ControlsWindow(base::EventQueue *in_user_event_queue = nullptr) :
        WindowData(in_user_event_queue)
    {
      m_window = nullptr;
      add_close_event_listener(get_user_event_queue());
    }
    // -------------------------------------------------------------------------
    // ~ControlsWindow
    // -------------------------------------------------------------------------
    ~ControlsWindow() override
    {
      SHL_DBG_OUT("ControlsWindow was deleted");
    }
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // wait_user_event
    // -------------------------------------------------------------------------
    size_t wait_user_event()
    {
      get_user_event_queue()->wait();
      return get_window_num();
    }
    // -------------------------------------------------------------------------
    // process_widget_events
    // -------------------------------------------------------------------------
    void process_widget_events(bool in_last_only = false)
    {
      get_user_event_queue()->process_events(in_last_only);
    }

  protected:
    // Member functions --------------------------------------------------------
    // -------------------------------------------------------------------------
    // create_window_object
    // -------------------------------------------------------------------------
    Gtk::Window *create_window_object(const char *in_title) override
    {
      m_window = new shl::gtk::controls::MainWindow(this, in_title);
      return m_window;
    }
    // -------------------------------------------------------------------------
    // get_window_object
    // -------------------------------------------------------------------------
    Gtk::Window *get_window_object() override
    {
      return m_window;
    }
    // -------------------------------------------------------------------------
    // delete_window_object
    // -------------------------------------------------------------------------
    void delete_window_object() override
    {
      delete m_window;
      m_window = nullptr;
    }
    // -------------------------------------------------------------------------
    // is_window_object_null
    // -------------------------------------------------------------------------
    bool is_window_object_null() override
    {
      if (m_window == nullptr)
        return true;
      return false;
    }
    // -------------------------------------------------------------------------
    // get_default_window_title
    // -------------------------------------------------------------------------
    const char *get_default_window_title() override
    {
      return "ControlsWindow";
    }
    // -------------------------------------------------------------------------
    // update_window
    // -------------------------------------------------------------------------
    void update_window() override
    {
      if (m_window == nullptr)
        return;
      // The following will call WindowData::update() at the end of the call chain
      m_window->update();
    }

  private:
    shl::gtk::controls::MainWindow *m_window;
  };
} // namespace shl::gtk

#endif  // #ifdef CONTROLS_WINDOW_GTK_H_
