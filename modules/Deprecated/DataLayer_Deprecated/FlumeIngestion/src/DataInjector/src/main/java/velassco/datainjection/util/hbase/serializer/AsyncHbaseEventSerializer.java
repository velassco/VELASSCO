package velassco.datainjection.util.hbase.serializer;

/*
002	 * Licensed to the Apache Software Foundation (ASF) under one
003	 * or more contributor license agreements.  See the NOTICE file
004	 * distributed with this work for additional information
005	 * regarding copyright ownership.  The ASF licenses this file
006	 * to you under the Apache License, Version 2.0 (the
007	 * "License"); you may not use this file except in compliance
008	 * with the License.  You may obtain a copy of the License at
009	 *
010	 * http://www.apache.org/licenses/LICENSE-2.0
011	 *
012	 * Unless required by applicable law or agreed to in writing,
013	 * software distributed under the License is distributed on an
014	 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
015	 * KIND, either express or implied.  See the License for the
016	 * specific language governing permissions and limitations
017	 * under the License.
018	 */

import java.util.List;
import org.apache.flume.Event;
import org.apache.flume.conf.Configurable;
import org.apache.flume.conf.ConfigurableComponent;
import org.hbase.async.AtomicIncrementRequest;
import org.hbase.async.PutRequest;
/**
* Interface for an event serializer which serializes the headers and body
* of an event to write them to hbase. This is configurable, so any config
* params required should be taken through this.
* The table should be valid on the column family. An implementation
* of this interface is expected by the {@linkplain AsyncHBaseSink} to serialize
* the events.
*/
public interface AsyncHbaseEventSerializer extends Configurable,
ConfigurableComponent {
/**
* Initialize the event serializer.
* @param table - The table the serializer should use when creating
* {@link org.hbase.async.PutRequest} or
* {@link org.hbase.async.AtomicIncrementRequest}.
* @param cf - The column family to be used.
*/
public void initialize(byte[] table, byte[] cf);
/**
* @param Event to be written to HBase.
*/
public void setEvent(Event event);
/**
* Get the actions that should be written out to hbase as a result of this
* event. This list is written to hbase.
* @return List of {@link org.hbase.async.PutRequest} which
* are written as such to HBase.
*
*
*/
public List<PutRequest> getActions();
/**
* Get the increments that should be made in hbase as a result of this
* event. This list is written to hbase.
* @return List of {@link org.hbase.async.AtomicIncrementRequest} which
* are written as such to HBase.
*
*
*/
public List<AtomicIncrementRequest> getIncrements();
/**
* Clean up any state. This will be called when the sink is being stopped.
*/
public void cleanUp();
}

