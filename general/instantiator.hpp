// Copyright (c) 2010, Lawrence Livermore National Security, LLC. Produced at
// the Lawrence Livermore National Laboratory. LLNL-CODE-443211. All Rights
// reserved. See file COPYRIGHT for details.
//
// This file is part of the MFEM library. For more information and source code
// availability see http://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License (as published by the Free
// Software Foundation) version 2.1 dated February 1999.

#ifndef MFEM_INSTANTIATOR_HPP
#define MFEM_INSTANTIATOR_HPP

#include <array>
#include <type_traits>
#include <unordered_map>

using std::decay;
using std::array;
using std::size_t;
using std::forward;
using std::enable_if;
using std::tuple_size;
using std::unordered_map;

template<typename Params, const std::size_t NID, typename Key, typename Kernel>
class Instantiator
{
private:
   using map_t = unordered_map<Key, Kernel>;
   map_t map;

   template<class T, size_t I, class = void>
   struct Kernels
   {
      static void add(const T& id, map_t &map)
      {
         constexpr Key key = GetKey<I>();
         constexpr Kernel value = GetValue<Kernel, key>();
         map.emplace(key, value);
         Kernels<T,I+1u>::add(forward<T>(id), map);
      }
   };

   template<class T, size_t I>
   struct Kernels<T, I,
             typename enable_if<I==tuple_size<
             typename decay<T>::type>::value>::type>
   {
      static void add(T&, map_t&) {}
   };

   template<class T>
   void foreach(T&& id) { Kernels<T,0u>::add(forward<T>(id), map); }

public:
   Instantiator(const Params &ids) { foreach(ids); }

   bool Find(const Key id)
   {
      return (map.find(id) != map.end()) ? true : false;
   }

   Kernel At(const Key id)
   {
      return map.at(id);
   }
};

#endif // MFEM_INSTANTIATOR_HPP