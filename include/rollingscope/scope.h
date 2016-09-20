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


#ifndef INCLUDED_ROLLINGSCOPE_SCOPE_H
#define INCLUDED_ROLLINGSCOPE_SCOPE_H

#include <Python.h>
#include <rollingscope/api.h>
#include <gnuradio/sync_block.h>
#include <QWidget>

namespace gr {
  namespace rollingscope {

    /*!
     * \brief <+description of block+>
     * \ingroup rollingscope
     *
     */
    class ROLLINGSCOPE_API scope : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<scope> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of rollingscope::scope.
       *
       * To avoid accidental use of raw pointers, rollingscope::scope's
       * constructor is in a private implementation
       * class. rollingscope::scope::make is the public interface for
       * creating new instances.
       */
      static sptr make(int num_sp, float fs, float full_scale,
                       QWidget* parent=NULL);

      virtual PyObject* pyqwidget() = 0;
    };

  } // namespace rollingscope
} // namespace gr

#endif /* INCLUDED_ROLLINGSCOPE_SCOPE_H */

