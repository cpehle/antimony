#pragma once
#include <Python.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <regex>
#include <set>

class Downstream;

class Root
{
public:
    virtual ~Root();

    /*
     *  Returns a new reference to the given object or NULL.
     *  May throw Proxy::Exception to indicate a special error.
     */
    virtual PyObject* pyGetAttr(std::string name, Downstream* caller,
                                uint8_t flags) const=0;

    /*
     *  Attempts to set the given attribute.
     *  Returns true on success.
     */
    virtual void pySetAttr(std::string name, PyObject* obj,
                           uint8_t flags)=0;

    /*
     *  Record that the given Downstream looked up a particular name
     */
    void saveLookup(std::string name, Downstream* caller);

    /*
     *  When a child is changed, call 'trigger' on all Downstream
     *  objects that have tried looking up this name before.
     */
    void changed(std::string n, uint32_t uid);

    /*
     *  Removes a Downstream from the map of failed lookups.
     */
    void removeDownstream(Downstream* d);

    /*
     *  Checks that the given name is valid
     *  (not a Python keyword and a valid variable name)
     */
    static bool isNameValid(std::string name);

    /*****************************************************/
    /*            TEMPLATED HELPER FUNCTIONS             */
    /*****************************************************/
    /*
     *  Helper function to install a new object into a list,
     *  finding a new unique ID number and returning it.
     */

    /*
     *  Helper function to look up a child by name.
     */
    template <class T>
    T* getByName(std::string n, const std::list<std::unique_ptr<T>>& ts) const
    {
        auto match = std::find_if(ts.begin(), ts.end(),
                                  [&](const std::unique_ptr<T>& t)
                                  { return t->name == n; });
        return (match == ts.end()) ? NULL : match->get();
    }

    /*
     *  Helper function to look up a child by UID.
     */
    template <class T>
    T* getByUID(uint32_t uid, const std::list<std::unique_ptr<T>>& ts) const
    {
        auto match = std::find_if(ts.begin(), ts.end(),
                                  [&](const std::unique_ptr<T>& t)
                                  { return t->uid == uid; });
        return (match == ts.end()) ? NULL : match->get();
    }

    /*
     *  Look up a child by either UID (if the key begins with __) or name.
     */
    template <class T>
    T* get(std::string n, const std::list<std::unique_ptr<T>>& ts) const
    {
        static std::regex r("__([[:digit:]]+)");
        std::smatch match;
        if (std::regex_match(n, match, r))
            return getByUID(std::stoull(match[1]), ts);
        else
            return getByName(n, ts);
    }

    /*
     *  Queue a Downstream object for re-evaluation.
     */
    virtual void queue(Downstream* d)=0;
    virtual void flushQueue()=0;

protected:

    template <class T>
    uint32_t install(T* t, std::list<std::unique_ptr<T>>* ts)
    {
        // Find the lowest unused unique ID number
        std::unordered_set<uint32_t> indices;
        for (const auto& ptr : *ts)
            indices.insert(ptr->uid);
        uint32_t uid = 0;
        while (indices.count(uid))
            uid++;

        ts->push_back(std::unique_ptr<T>(t));
        return uid;
    }

    /*
     *  Delete a particular value from a list, calling 'changed'
     *  with its name and UID to inform others of its deletion.
     */
    template <class T>
    void uninstall(T* t, std::list<std::unique_ptr<T>>* ts)
    {
        const auto name = t->name;
        const auto uid = t->uid;
        ts->remove_if([&](const std::unique_ptr<T>& t_)
                      { return t_.get() == t; });
        changed(name, uid);
    }

    std::unordered_multimap<std::string, Downstream*> lookups;
};
