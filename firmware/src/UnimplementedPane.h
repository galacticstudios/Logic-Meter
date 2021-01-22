/* 
 * File:   UnimplementedPane.h
 * Author: Bob
 *
 * Created on January 10, 2021, 11:23 AM
 */

#ifndef UNIMPLEMENTEDPANE_H
#define	UNIMPLEMENTEDPANE_H

#include "Pane.h"

class UnimplementedPane : public Pane
{
public:
    UnimplementedPane() {}
    virtual ~UnimplementedPane() {}
        
    void Update() {}

protected:
    virtual laWidget *GetWidget() const;

private:
    UnimplementedPane(const UnimplementedPane& orig);
};

#endif	/* UNIMPLEMENTEDPANE_H */

