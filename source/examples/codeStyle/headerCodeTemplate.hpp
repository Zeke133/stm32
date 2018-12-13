/**
 * @file    headerCodeTemplate.hpp
 * @author  Denis Homutovski
 * @version V1.0.0
 * @date    12-12-2018
 * @brief   Class header template.
 * @details   Class header code-style template.
 * @pre       -
 * @bug       -
 * @warning   -
 * @copyright GNU Public License.
 */

#ifndef _HEADER_CODE_TEMPLATE_H
#define _HEADER_CODE_TEMPLATE_H

// using
#include <stdint.h>
extern "C" {
//#include <stdint.h>
}

// IRQ handlers. Extern "C" macro is needed for correct link procedure.
extern "C" {
    void InterruptHandlerFunction(void);
}

/**
 * Class header code-style template.
 * Here put detailed description ...
 * ...
 */
class ClassTempate {

// Interrupt handlers need access to 'callbacks'.
friend void InterruptHandlerFunction(void);

public:

    /**
     * Enumeration
     */
    enum class Enumeration : uint8_t {
        element1    = 0xF4,     /// element1 description
        LAST_ELEMENT
    };

    ClassTempate(/*args*/);
    ClassTempate(const ClassTempate&) = delete;             /**< Delete copy constructor. */
    ClassTempate& operator=(const ClassTempate&) = delete;  /**< Delete assignment operator. */

    void publicFunction(void) const;

private:

    void privateFunction(void);

    /**
     * Private struct or bit-field example
     */
    struct PrivateBitField {

        uint32_t element1 : 2;
        /**< description of element1 */
    };

    /** Private constant. */ 
    const uint32_t constant = 1000;

    static uint8_t staticAtribute1;     /**< Description. */
    static uint8_t staticAtribute2;     /**< Description. */
    
    /**
     * long description
     * of attribute
     */
    uint8_t * someAttribute;
    
    PrivateBitField structAtribute;     /**< Attribute. */
    
};

#endif
