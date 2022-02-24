//
// Filename : AssignerEventList.java
//
// UK Crown Copyright (c) 2005. All Rights Reserved
//
package org.xtuml.masl.inspector.gui;

import java.awt.Dimension;
import java.util.Arrays;

import javax.swing.JPopupMenu;

import org.xtuml.masl.inspector.processInterface.EventMetaData;
import org.xtuml.masl.inspector.processInterface.ObjectMetaData;

public class AssignerEventList extends DependentObjectList<EventMetaData, ObjectMetaData> {

    /**
     *
     */
    private static final long serialVersionUID = 1L;

    public AssignerEventList(final ObjectList objectList) {
        super(new DependentObjectListModel<EventMetaData, ObjectMetaData>(objectList) {

            /**
             *
             */
            private static final long serialVersionUID = 1L;

            @Override
            protected EventMetaData[] getDependentValues(final ObjectMetaData dependee) {
                final EventMetaData[] events = dependee.getAssignerEvents();
                Arrays.sort(events);
                return events;
            }

        });
        addMouseListener(new ListPopupHandler(getPopup()));

    }

    @Override
    public Dimension getPreferredScrollableViewportSize() {
        final Dimension d = super.getPreferredScrollableViewportSize();

        d.width = getCellRenderer()
                .getListCellRendererComponent(this, "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", 0, true, true)
                .getPreferredSize().width;

        return d;
    }

    private JPopupMenu getPopup() {
        return null;
    }

}
