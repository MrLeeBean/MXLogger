// automatically generated by the FlatBuffers compiler, do not modify
// ignore_for_file: unused_import, unused_field, unused_local_variable

import 'dart:typed_data' show Uint8List;
import 'package:mxlogger_analyzer_lib/src/dependencies/flat_buffers/flat_buffers.dart' as fb;



class LogSerialize {
  LogSerialize._(this._bc, this._bcOffset);
  factory LogSerialize(List<int> bytes) {
    fb.BufferContext rootRef =  fb.BufferContext.fromBytes(bytes);
    return reader.read(rootRef, 0);
  }

  static const fb.Reader<LogSerialize> reader =  _LogSerializeReader();

  final fb.BufferContext _bc;
  final int _bcOffset;

  String get name => const fb.StringReader().vTableGet(_bc, _bcOffset, 4, "");
  String get tag => const fb.StringReader().vTableGet(_bc, _bcOffset, 6, "");
  String get msg => const fb.StringReader().vTableGet(_bc, _bcOffset, 8, "");
  int get level => const fb.Int8Reader().vTableGet(_bc, _bcOffset, 10, 0);
  int get threadId => const fb.Int32Reader().vTableGet(_bc, _bcOffset, 12, 0);
  int get isMainThread => const fb.Uint8Reader().vTableGet(_bc, _bcOffset, 14, 0);
  int get timestamp => const fb.Uint64Reader().vTableGet(_bc, _bcOffset, 16, 0);

  @override
  String toString() {
    return 'log_serialize{name: $name, tag: $tag, msg: $msg, level: $level, threadId: $threadId, isMainThread: $isMainThread, timestamp: $timestamp}';
  }
}

class _LogSerializeReader extends fb.TableReader<LogSerialize> {
  const _LogSerializeReader();

  @override
  LogSerialize createObject(fb.BufferContext bc, int offset) =>
       LogSerialize._(bc, offset);
}

class LogSerializeBuilder {
  LogSerializeBuilder(this.fbBuilder) {
    assert(fbBuilder != null);
  }

  final fb.Builder fbBuilder;

  void begin() {
    fbBuilder.startTable(7);
  }

  int addNameOffset(int offset) {
    fbBuilder.addOffset(0, offset);
    return fbBuilder.offset;
  }
  int addTagOffset(int offset) {
    fbBuilder.addOffset(1, offset);
    return fbBuilder.offset;
  }
  int addMsgOffset(int offset) {
    fbBuilder.addOffset(2, offset);
    return fbBuilder.offset;
  }
  int addLevel(int level) {
    fbBuilder.addInt8(3, level);
    return fbBuilder.offset;
  }
  int addThreadId(int threadId) {
    fbBuilder.addInt32(4, threadId);
    return fbBuilder.offset;
  }
  int addIsMainThread(int isMainThread) {
    fbBuilder.addUint8(5, isMainThread);
    return fbBuilder.offset;
  }
  int addTimestamp(int timestamp) {
    fbBuilder.addUint64(6, timestamp);
    return fbBuilder.offset;
  }

  int finish() {
    return fbBuilder.endTable();
  }
}

// class LogSerializeObjectBuilder extends fb.ObjectBuilder {
//   final String _name;
//   final String _tag;
//   final String _msg;
//   final int _level;
//   final int _threadId;
//   final int _isMainThread;
//   final int _timestamp;
//
//   LogSerializeObjectBuilder({
//     String name,
//     String tag,
//     String msg,
//     int level,
//     int threadId,
//     int isMainThread,
//     int timestamp,
//   })
//       : _name = name,
//         _tag = tag,
//         _msg = msg,
//         _level = level,
//         _threadId = threadId,
//         _isMainThread = isMainThread,
//         _timestamp = timestamp;
//
//   /// Finish building, and storage into the [fbBuilder].
//   @override
//   int finish(
//       fb.Builder fbBuilder) {
//     assert(fbBuilder != null);
//     final int nameOffset = fbBuilder.writeString(_name);
//     final int tagOffset = fbBuilder.writeString(_tag);
//     final int msgOffset = fbBuilder.writeString(_msg);
//
//     fbBuilder.startTable();
//     if (nameOffset != null) {
//       fbBuilder.addOffset(0, nameOffset);
//     }
//     if (tagOffset != null) {
//       fbBuilder.addOffset(1, tagOffset);
//     }
//     if (msgOffset != null) {
//       fbBuilder.addOffset(2, msgOffset);
//     }
//     fbBuilder.addInt8(3, _level);
//     fbBuilder.addInt32(4, _threadId);
//     fbBuilder.addUint8(5, _isMainThread);
//     fbBuilder.addUint64(6, _timestamp);
//     return fbBuilder.endTable();
//   }
//
//
// }
