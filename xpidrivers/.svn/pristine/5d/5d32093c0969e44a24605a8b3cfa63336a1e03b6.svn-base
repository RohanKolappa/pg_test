#ifndef _PROPERTY_TABLE_H_
#define _PROPERTY_TABLE_H_

// System headers
#include <map>
#include <string>
#include <vector>

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides a general-purpose key/value table for storing properties.
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class PropertyTable {

    // Constructor / Destructor
  public:
    
    /**
     * Default constructor
     */
    PropertyTable(void);
    
    /**
     * Creates a property table with the single passed key / value pair
     * @param property - Property "key"
     * @param value - Value for the property
     */
    PropertyTable(const std::string& property, const std::string& value);
    
    /**
     * Copy constructor
     * @param other - Instance to be copied
     */
    PropertyTable(const PropertyTable& other);
    
    /**
     * Virtual destructor
     */
    virtual ~PropertyTable(void);
    
    // Public interface methods
  public:
    
    /**
     * Returns a vector of property keys
     * @return - The vector of keys
     */
    const std::vector<std::string> getPropertyKeys(void) const;
    
    /**
     * Returns true if the passed property exists in the table
     * @param property - The property to be searched for
     * @return - True if the property exists, false otherwise
     */
    const bool hasProperty(const std::string& property) const;
    
    /**
     * Returns the value of the specified property, or the empty string
     * if it does not exist
     * @param property - The property to be examined
     * @return - The value of the requested property, or the empty string
     */
    const std::string getProperty(const std::string& property) const;
    
    /**
     * Sets the value of the specified property
     * @param property - The property to set
     * @param value - The value to be assigned to the property
     */
    void setProperty(const std::string& property, const std::string& value);
    
    /**
     * Clears all properties from the instance
     */
    void clear(void);
    
    /**
     * Returns true if the instance is empty
     * @return - True if the instance is empty, false otherwise
     */
    const bool isEmpty(void) const;
    
    // Public overloaded operators
  public:
    
    /**
     * Assignment operator
     * @param rhs - The "right-hand side" instance to be assigned from
     * @return - A reference to this instance
     */
    const PropertyTable& operator=(const PropertyTable& rhs);
    
    // Private attributes
  private:
    
    /**
     * Encapsulated associative map of properties to values
     */
    std::map<std::string, std::string> properties;
  };
}

#endif
