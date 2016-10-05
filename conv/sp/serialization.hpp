#pragma once
#include "sp/complex.hpp"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/vector.hpp>
//#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/split_member.hpp>

#include <memory>
#include <deque>
#include <set>


#define SER_NVP(name, value) boost::serialization::make_nvp(BOOST_PP_STRINGIZE(name), value)

namespace boost { namespace serialization
{
    template<class Archive, class real>
    void serialize(Archive & ar, sp::complex_tmpl<real> &v, const unsigned int version)
    {
        (void) version;

        ar & SER_NVP(cre, v.re());
        ar & SER_NVP(cim, v.im());
    }
}}


namespace boost { namespace serialization
{
    template<class Archive, class T>
    inline void save(
            Archive & ar,
            const std::unique_ptr< T > &t,
            const unsigned int /*file_version*/
            )
    {
        // only the raw pointer has to be saved
        const T * const base_pointer = t.get();
        ar & BOOST_SERIALIZATION_NVP(base_pointer);
    }

    template<class Archive, class T>
    inline void load(
            Archive & ar,
            std::unique_ptr< T > &t,
            const unsigned int /*file_version*/
            )
    {
        T *base_pointer;
        ar & BOOST_SERIALIZATION_NVP(base_pointer);
        t.reset(base_pointer);
    }

    template<class Archive, class T>
    inline void serialize(
            Archive & ar,
            std::unique_ptr< T > &t,
            const unsigned int file_version
            )
    {
        boost::serialization::split_free(ar, t, file_version);
    }

    template<class Archive, class T, class D, class Allocator>
    inline void save(Archive & ar, const std::deque<std::unique_ptr<T, D>, Allocator> &t,const unsigned int version)
    {
        (void) version;

        collection_size_type count (t.size());
        ar << SER_NVP(count, count);
        for(auto it=t.begin(), end=t.end(); it!=end; ++it)
        {
            ar << boost::serialization::make_nvp("item", (*it));
        }
    }

    template<class Archive, class T, class D, class Allocator>
    inline void load(Archive & ar, std::deque<std::unique_ptr<T, D>, Allocator> &t, const unsigned int version)
    {
        (void) version;

        collection_size_type count;
        ar >> SER_NVP(count, count);
        t.clear();
        while( count-- > 0 )
        {
            std::unique_ptr<T, D> i;
            ar >> boost::serialization::make_nvp("item", i);
            t.push_back(std::move(i)); // use std::move
        }
    }

    template<class Archive, class T, class D, class Allocator>
    inline void save(Archive & ar, const std::vector<std::unique_ptr<T, D>, Allocator> &t,const unsigned int version)
    {
        (void) version;

        collection_size_type count (t.size());
        ar << SER_NVP(count, count);
        for(auto it=t.begin(), end=t.end(); it!=end; ++it)
        {
            ar << boost::serialization::make_nvp("item", (*it));
        }
    }

    template<class Archive, class T, class D, class Allocator>
    inline void load(Archive & ar, std::vector<std::unique_ptr<T, D>, Allocator> &t, const unsigned int version)
    {
        (void) version;

        collection_size_type count;
        ar >> SER_NVP(count, count);
        t.clear();
        while( count-- > 0 )
        {
            std::unique_ptr<T, D> i;
            ar >> boost::serialization::make_nvp("item", i);
            t.push_back(std::move(i)); // use std::move
        }
    }

}}




