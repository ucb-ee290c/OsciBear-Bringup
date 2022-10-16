import chisel3._
import chisel3.util._
import chisel3.tester._
import chisel3.tester.RawTester.test
import scala.collection._

//     parameter SYSCLK = 100_000_000,
//     parameter WIDTH = 123,
//     parameter BAUD = 2_000_000
class TSIToUart(SYSCLK: Int = 100000000, WIDTH: Int = 123, BAUD: Int = 2000000) extends Module {
//     input clk,
//     input rst,
//     output uart_tx,
//     input tl_rising_clk,
//     output tl_in_rd,
//     input tl_in_data,
//     input tl_in_valid
    val io = IO(new Bundle {
        val uart_tx = Output(UInt(1.W))
        val tl_rising_clk = Input(UInt(1.W))
        val tl = Flipped(Decoupled(UInt(1.W)))
    })
    
//     localparam BAUD_CYCLE = SYSCLK / BAUD;
//     localparam BYTE_WIDTH = (WIDTH-1)/8 + 1;
    val BAUD_CYCLE = SYSCLK / BAUD;
    val BYTE_WIDTH = (WIDTH - 1) / 8 + 1;
    
//     wire [4-1:0] packet_counter_Q;
//     wire [16-1:0] countdown_Q;
//     wire uart_fire, uart_idle, sreg_isOut, sreg_out;
    val packet_counter_Q = RegInit(0.U(4.W));
    val countdown_Q = RegInit(0.U(16.W));
    val uart_fire = Wire(Bool())
    val uart_idle = Wire(Bool())
    val sreg_isOut = Wire(Bool())
    val sreg_out = Wire(UInt(1.W))
    
    val sreg0 = new ShiftReg()
    
    
//     assign uart_tx = (packet_counter_Q == 0) ? uart_idle : ((packet_counter_Q == 9) ? 1'b1 : sreg_out);
//     assign uart_fire = (countdown_Q == BAUD_CYCLE);
//     assign tl_in_rd = !sreg_isOut;
//     assign uart_idle = (packet_counter_Q == 0) && !sreg_isOut && (countdown_Q == 16'd0);
    uart_fire := countdown_Q === BAUD_CYCLE.U
    uart_idle := (packet_counter_Q === 0.U) && !sreg_isOut && countdown_Q === 0.U
    
    io.tl.ready := !sreg_isOut
    when (packet_counter_Q === 0.U) {
        io.uart_tx := uart_idle
    }
    .elsewhen (packet_counter_Q === 9.U) {
        io.uart_tx := 1.U
    }
    .otherwise {
        io.uart_tx := sreg_out
    }
    
//     RegInit #(.WIDTH(4), .INIT(0)) 
//     packet_counter(
//         .clk(clk), .rst(rst || uart_idle || ((packet_counter_Q == 9) && uart_fire)), .en(uart_fire),
//         .D(packet_counter_Q + 1'b1),
//         .Q(packet_counter_Q)
//     );
    when (uart_idle || ((packet_counter_Q === 9.U) && uart_fire)) {
        packet_counter_Q := 0.U
    }
    .elsewhen (uart_fire) {
        packet_counter_Q := packet_counter_Q + 1.U
    }
    
//     RegInit #(.WIDTH(16), .INIT(0)) 
//     countdown(
//         .clk(clk), .rst(rst || uart_fire || uart_idle), .en(1),
//         .D(countdown_Q + 1'b1),
//         .Q(countdown_Q)
//     );
    when (uart_fire || uart_idle) {
        countdown_Q := 0.U
    }
    .otherwise {
        countdown_Q := countdown_Q + 1.U
    }
    
//     shiftReg #(.IN_WIDTH(WIDTH), .OUT_WIDTH(BYTE_WIDTH*8), .D_WIDTH(BYTE_WIDTH*8)) 
//     sreg0(
//         .clk(clk), .rst(rst), 
//         .en(sreg_isOut ? (uart_fire && !((packet_counter_Q == 0) || (packet_counter_Q == 9))) : (tl_rising_clk & tl_in_valid)), 
//         .D(tl_in_data),

//         .mode(sreg_isOut), // 0: shifting in, 1: shifting out
//         .Q(sreg_out)
//     );
    when (sreg_isOut) {
        sreg0.io.en := uart_fire && !((packet_counter_Q === 0.U) || (packet_counter_Q === 9.U))
    }
    .otherwise {
        sreg0.io.en := io.tl_rising_clk & io.tl.valid
    }
    sreg0.io.D := io.tl.bits
    sreg_isOut := sreg0.io.mode
    sreg_out := sreg0.io.Q
    
}
