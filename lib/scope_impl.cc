/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "scope_impl.h"

#include <gnuradio/io_signature.h>
#include <iostream>
#include <string>

namespace gr {
  namespace rollingscope {

    scope::sptr
    scope::make(int num_sp, float fs, float full_scale, QWidget* parent)
    {
      return gnuradio::get_initial_sptr
        (new scope_impl(num_sp, fs, full_scale, parent));
    }

    /*
     * The private constructor
     */
    scope_impl::scope_impl(int num_sp, float fs, float full_scale,
                           QWidget* parent)
      : QWidget(parent),
      gr::sync_block("scope",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(0, 0, 0)),
      num_sp_disp(num_sp),
      fs(fs), full_scale(full_scale)
    {
      if(qApp != NULL) {
        d_qApplication = qApp;
      } else {
        int d_argc = 1;
        char *d_argv = new char;
        d_argv[0] = '\0';
        d_qApplication = new QApplication(d_argc, &d_argv);
      }

      this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

      QTimer *timer = new QTimer(this);
      connect(timer, SIGNAL(timeout()), this, SLOT(timer_fired()));
      timer->start(100);

      this->updateGeometry();
      set_max_noutput_items(num_sp_disp);
    }

    /*
     * Our virtual destructor.
     */
    scope_impl::~scope_impl()
    {
    }

    void scope_impl::timer_fired()
    {
        this->repaint();
    }

    void scope_impl::paintEvent(QPaintEvent *event)
    {
      QPainter p(this);
      QColor color_bg(50, 50, 50);
      QColor color_line(0,255,100);
      QPen pen_line(color_line);

      int w = this->size().width();
      int h = this->size().height();

      p.fillRect(0, 0, w, h, color_bg);
      p.setPen(pen_line);

      int i = 0;
      float x = 0;

      mtx.lock();
      std::list<float> paint_buffer(time_series);
      //std::cout << "paint " << std::endl;
      mtx.unlock();

      float x_step = 1;
      if(num_sp_disp < paint_buffer.size()) {
        x_step = ((float)w / (float)paint_buffer.size());
      } else {
        x_step = ((float)w / (float)num_sp_disp);
      }
//      std::cout << "buffer: " << paint_buffer.size() << std::endl;
//      std::cout << "w: " << w << " h: " << h << " step: " << x_step << std::endl;
      for(int i = 0; i < 10; i++) {
        float tick_x = (i * w / 9.0);
        float tick_time = (tick_x / x_step) / fs;
        p.drawText((int)tick_x, h - 20, QString::number(tick_time));
      }
      std::list<float>::iterator it;
      int latest_x_pos = 1;
      int latest_x_tick_pos = -1000;
      for(it = paint_buffer.begin(); it != paint_buffer.end(); it++) {
        if(latest_x_pos != (int)x) {
          p.drawPoint((int)x, ((h/2.0) - (*it)*((h/2.0) / full_scale)));
        }
        latest_x_pos = (int)x;
        x += x_step;
      }
    }

    PyObject* scope_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)((QWidget*)this));
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }

    int
    scope_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];

      // Do <+signal processing+>
      mtx.lock();
      int skip = 0;
      //std::cout << "work: n: " << noutput_items << " win size: " << time_series.size() << std::endl;
      if((time_series.size() + noutput_items) > num_sp_disp) {
        //std::cout << "window too full for new data set" << std::endl;
        int num_excess = (time_series.size() + noutput_items) - num_sp_disp;
        if(num_excess > num_sp_disp) {
          num_excess = num_sp_disp;
          skip = noutput_items - num_sp_disp;
        }
        std::list<float>::iterator delete_until = time_series.begin();
        //std::cout << "del: " << num_excess << std::endl;
        std::advance(delete_until, num_excess); //erase [begin, end)
        time_series.erase(time_series.begin(), delete_until);
      }
      //std::cout << "skip: " << skip << std::endl;
      time_series.insert(time_series.end(), in + skip, in + noutput_items);
      mtx.unlock();

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace rollingscope */
} /* namespace gr */

