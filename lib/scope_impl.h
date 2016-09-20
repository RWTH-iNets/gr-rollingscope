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

#ifndef INCLUDED_ROLLINGSCOPE_SCOPE_IMPL_H
#define INCLUDED_ROLLINGSCOPE_SCOPE_IMPL_H

#include <rollingscope/scope.h>
#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <list>
#include <boost/thread.hpp>

namespace gr {
  namespace rollingscope {

    class scope_impl : public QWidget, public scope
    {
     Q_OBJECT

    private:
      std::list<float> time_series;
      int num_sp_disp;
      QApplication *d_qApplication;
      boost::mutex mtx;
      float fs;
      float full_scale;

    public:
      scope_impl(int num_sp, float fs, float full_scale, QWidget* parent);
      ~scope_impl();
      PyObject* pyqwidget();
      void paintEvent(QPaintEvent *event);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

    public slots:
      void timer_fired();
    };

  } // namespace rollingscope
} // namespace gr

#endif /* INCLUDED_ROLLINGSCOPE_SCOPE_IMPL_H */

