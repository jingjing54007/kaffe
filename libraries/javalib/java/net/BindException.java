/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

public class BindException extends SocketException {

  public BindException(String msg)
  {
    super(msg);
  }

  public BindException()
  {
    super();
  }

}
