import { json } from '@sveltejs/kit';
import type { RequestHandler } from '@sveltejs/kit';

import nodemailer from 'nodemailer';
import { PUBLIC_NODEMAILER_USER, PUBLIC_NODEMAILER_PASS } from '$env/static/public';

export const POST: RequestHandler = async ({ request, locals: { supabase } }) => {
  const body = await request.json();

  const { mac, state } = body;

  if (!mac) {
    return json({ error: 'MAC Address is required' }, { status: 400 });
  }

  if (!state) {
    return json({ error: 'State is required' }, { status: 400 });
  }

  const { data, error } = await supabase
    .from('devices')
    .select(`*, profiles(email)`)
    .eq('mac', mac)
    .is('connected', true)
    .maybeSingle();

  if (error) {
    return json({ error: error.message }, { status: 500 });
  }

  if (!data) {
    return json({ error: 'Device not found' }, { status: 404 });
  }

  const { profiles: { email } } = data;

  const transport = nodemailer.createTransport({
    host: 'smtp.gmail.com',
    port: 465,
    secure: true,
    auth: {
      user: PUBLIC_NODEMAILER_USER,
      pass: PUBLIC_NODEMAILER_PASS
    },
  })

  const info = await transport.sendMail({
    to: email,
    subject: "FireWall Alert",
    html: `
      <h1>FireWall Alert</h1>
      <p>MAC Address: ${mac}</p>
      <p>State: ${state}</p>
    `,
  });

  console.log("Message sent: %s", info.messageId);
  
  return json(data);
}