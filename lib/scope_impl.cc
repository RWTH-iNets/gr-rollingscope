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

namespace gr {
  namespace rollingscope {

    scope::sptr
    scope::make(int num_sp, float fs, QWidget* parent)
    {
      return gnuradio::get_initial_sptr
        (new scope_impl(num_sp, fs, parent));
    }

    /*
     * The private constructor
     */
    scope_impl::scope_impl(int num_sp, float fs, QWidget* parent)
      : QWidget(parent),
      gr::sync_block("scope",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(0, 0, 0)),
      num_sp_disp(num_sp),
      fs(fs)
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
      mtx.unlock();

      std::list<float>::iterator it;
      float x_step = 1;
      if(num_sp_disp < this->paint_buffer.size()) {
        x_step = ((float)w / (float) paint_buffer.size());
      } else {
        x_step = ((float)w / (float) num_sp_disp);
      }
      int latest_x_pos = 1;
      for(it = this->paint_buffer.begin(); it != this->paint_buffer.end(); it++) {
        if(latest_x_pos != (int)x) {
          p.drawPoint((int)x, ((h/2.0) - (*it)*(h/2.0)));
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
      if((time_series.size() + noutput_items) > num_sp_disp) {
        std::cout << "list too full " << std::endl;
        int num_excess = (time_series.size() + noutput_items) - num_sp_disp;
        if(num_excess > time_series.size()) {
          time_series.clear();
          std::cout << "erasing list" << std::endl;

        } else {
          std::cout << "updating list" << std::endl;
          std::list<float>::iterator list_front = time_series.begin();
          std::advance(list_front, num_excess);
          time_series.erase(time_series.begin(), list_front);
        }
      }
      time_series.insert(time_series.end(), in, in + noutput_items);
      mtx.unlock();

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace rollingscope */
} /* namespace gr */

